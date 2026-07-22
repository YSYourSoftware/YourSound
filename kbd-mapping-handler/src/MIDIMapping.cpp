#include "YSKMH/MIDIMapping.hpp"

#include <RtMidi.h>
#include <nlohmann/json.hpp>

using namespace YSMKH;

MIDIMapping::MIDIMapping(const std::string &json_string, const std::filesystem::path &script_folder,
						 const param_set_callback_t &param_set_callback, const param_get_callback_t &param_get_callback,
						 const cursor_move_by_callback_t &cursor_move_by_callback,
						 const cursor_move_to_callback_t &cursor_move_to_callback,
						 const cursor_load_callback_t &cursor_load_callback,
						 const cursor_back_callback_t &cursor_back_callback, const note_on_callback_t &note_on_callback,
						 const note_off_callback_t &note_off_callback) :
	m_param_set_callback(param_set_callback), m_param_get_callback(param_get_callback),
	m_cursor_move_by_callback(cursor_move_by_callback), m_cursor_move_to_callback(cursor_move_to_callback),
	m_cursor_load_callback(cursor_load_callback), m_cursor_back_callback(cursor_back_callback),
	m_note_on_callback(note_on_callback), m_note_off_callback(note_off_callback) {

	m_midi_in = new RtMidiIn();
	m_midi_in->ignoreTypes(true, true, true);

	nlohmann::json json = nlohmann::json::parse(json_string);

	if (!json["script"].is_string()) throw std::invalid_argument("json[\"script\"] must be a string");
	if (!json["controls"].is_object()) throw std::invalid_argument("json[\"controls\"] must be an object");

	if (json["script"].get<std::string>() != "null") {
		sol::table controller_interface = m_lua.create_named_table("controller");
		controller_interface["queryCC"] = [this](const uint8_t cc) -> lua_Number {
			return m_midi_cc_values[cc] / 127.0;
		};

		controller_interface["bind"] = [this](const std::string &action, const sol::function &callback) {
			m_control_to_lua_bindings[action] = callback;
		};

		sol::table input_player_interface = m_lua.create_named_table("inputPlayer");
		input_player_interface["updateParam"] = [this](const std::string &param, const lua_Number value) {
			m_param_set_callback(param, std::clamp(value, 0.0, 1.0));
		};

		input_player_interface["getParam"] = [this](const std::string &param) -> float {
			return m_param_get_callback(param);
		};

		sol::table patch_explorer_interface = m_lua.create_named_table("patchExplorer");
		patch_explorer_interface["moveBy"] = [this](const int8_t value) { m_cursor_move_by_callback(value); };
		patch_explorer_interface["moveTo"] = [this](const int8_t value) { m_cursor_move_to_callback(value); };
		patch_explorer_interface["load"] = [this] { m_cursor_load_callback(); };
		patch_explorer_interface["back"] = [this] { m_cursor_back_callback(); };

		m_lua.open_libraries(sol::lib::base, sol::lib::math);
		m_lua.script_file((script_folder / json["script"].get<std::string>()).generic_string());
	}

	for (auto it = json["controls"].begin(); it != json["controls"].end(); ++it) {
		if (!it->is_object()) throw std::invalid_argument("All members of json[\"controls\"] must be objects");

		if (!it.value()["bind"].is_string()) throw std::invalid_argument("Each control \"bind\" must be a string");
		if (!it.value()["onInput"].is_string())
			throw std::invalid_argument("Each control \"onInput\" must be a string");

		MIDIBindType bind_type;
		uint8_t bind_data;

		InputActionType action_type;
		std::string forward_id;

		{
			const std::string type_and_data = it.value()["bind"].get<std::string>();
			const auto pos = type_and_data.find(':');

			std::string type;
			std::string data;

			if (pos == std::string::npos) {
				type = type_and_data;
				data = "";
			} else {
				type = type_and_data.substr(0, pos);
				data = type_and_data.substr(pos + 1);
			}

			if (type == "cc") {
				bind_type = CC;
				bind_data = std::stoi(data);
			} else if (type == "note") {
				bind_type = NOTE;
				bind_data = std::stoi(data);
			} else throw std::invalid_argument(std::format("Unrecognised bind type: {}", type));
		}

		{
			const std::string type_and_data = it.value()["onInput"].get<std::string>();

			if (const auto pos = type_and_data.find(':'); pos == std::string::npos) {
				if (type_and_data != "scriptEvent")
					throw std::invalid_argument("onInput missing : when type is not scriptEvent");
				action_type = SCRIPT_EVENT;
			} else {
				if (type_and_data.substr(0, pos) != "forward")
					throw std::invalid_argument("onInput has : when type is not forward");
				action_type = FORWARD;
				forward_id = type_and_data.substr(pos + 1);
			}
		}

		m_midi_to_control_bindings[it.key()] = {
			.forward_id = forward_id, .bind_data = bind_data, .bind_type = bind_type, .action_type = action_type};
	}
}

MIDIMapping::~MIDIMapping() {
	m_control_to_lua_bindings.clear();
	delete m_midi_in;
}

void MIDIMapping::poll() {
	std::vector<uint8_t> message;
	while (true) {
		enum MIDIEventStatus : uint8_t {
			NOTE_OFF = 0x80,
			NOTE_ON = 0x90,
			POLYPHONIC_AFTERTOUCH = 0xA0,
			CONTROL_CHANGE = 0xB0,
			PROGRAM_CHANGE = 0xC0,
			CHANNEL_AFTERTOUCH = 0xD0,
			PITCH_WHEEL = 0xE0
		};

		m_midi_in->getMessage(&message);
		if (message.empty()) break;

		const uint8_t channel = message[0] & 0x0F;
		bool break_case = false;
		switch (message[0] & 0xF0) {
		case NOTE_OFF:
			if (channel == 15) {
				for (const auto &[id, binding] : m_midi_to_control_bindings)
					if (binding.bind_type == NOTE && binding.bind_data == message[1]) {
						if (binding.action_type == FORWARD) m_param_set_callback(binding.forward_id, 0.f);
						else if (binding.action_type == SCRIPT_EVENT) {
							if (!m_control_to_lua_bindings.contains(id))
								throw std::runtime_error(
									std::format("No callback found for defined script event: {}", id));
							m_control_to_lua_bindings[id](0);
						}
					}

				break;
			}

			m_note_off_callback(channel, message[1]);
			break;
		case NOTE_ON:
			if (channel == 15) {
				for (const auto &[id, binding] : m_midi_to_control_bindings)
					if (binding.bind_type == NOTE && binding.bind_data == message[1]) {
						if (binding.action_type == FORWARD) m_param_set_callback(binding.forward_id, 1.f);
						else if (binding.action_type == SCRIPT_EVENT) {
							if (!m_control_to_lua_bindings.contains(id))
								throw std::runtime_error(
									std::format("No callback found for defined script event: {}", id));
							m_control_to_lua_bindings[id](1);
						}
					}

				break;
			}

			m_note_on_callback(channel, message[1], message[2] / 127.f);
			break;
		case CONTROL_CHANGE:
			for (const auto &[id, binding] : m_midi_to_control_bindings)
				if (binding.bind_type == CC && binding.bind_data == message[1]) {
					if (binding.action_type == FORWARD) m_param_set_callback(binding.forward_id, message[2] / 127.f);
					else if (binding.action_type == SCRIPT_EVENT) {
						if (!m_control_to_lua_bindings.contains(id))
							throw std::runtime_error(std::format("No callback found for defined script event: {}", id));
						m_control_to_lua_bindings[id](message[2] / 127.0);
					}

					break_case = true;
					break;
				}

			if (break_case) break;

			m_param_set_callback(std::format("_midi_cc_{}", +message[1]), message[2] / 127.f);
			break;
		case PITCH_WHEEL:
			m_param_set_callback("_pitch_wheel",
								 (message[1] + (static_cast<uint16_t>(message[2]) << 7)) / static_cast<float>(2 << 14));
			break;
		default: break;
		}
	}
}

void MIDIMapping::set_midi_input(const uint32_t index) const { m_midi_in->openPort(index); }

std::vector<std::string> MIDIMapping::get_midi_input_names() const {
	std::vector<std::string> result;
	result.reserve(m_midi_in->getPortCount());

	for (uint32_t i = 0; i < m_midi_in->getPortCount(); i++) result.emplace_back(m_midi_in->getPortName(i));

	return result;
}
