#pragma once

#include "YSKMH/Export.hpp"

#include <array>
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>

#include <sol/sol.hpp>

namespace rt::midi {
	class RtMidiIn;
}

namespace YSKMH {
	class YSKMH_API MIDIMapping {
	public:
		typedef std::function<void(const std::string &id, float value)> param_set_callback_t;
		typedef std::function<float(const std::string &id)> param_get_callback_t;

		typedef std::function<void(int8_t amount)> cursor_move_by_callback_t;
		typedef std::function<void(int8_t position)> cursor_move_to_callback_t;
		typedef std::function<void()> cursor_load_callback_t;
		typedef std::function<void()> cursor_back_callback_t;

		typedef std::function<void(uint8_t channel, uint8_t note, float velocity)> note_on_callback_t;
		typedef std::function<void(uint8_t channel, uint8_t note)> note_off_callback_t;

		explicit MIDIMapping(const std::string &json_string, const std::filesystem::path &script_folder,
							 const param_set_callback_t &param_set_callback,
							 const param_get_callback_t &param_get_callback,
							 const cursor_move_by_callback_t &cursor_move_by_callback,
							 const cursor_move_to_callback_t &cursor_move_to_callback,
							 const cursor_load_callback_t &cursor_load_callback,
							 const cursor_back_callback_t &cursor_back_callback,
							 const note_on_callback_t &note_on_callback, const note_off_callback_t &note_off_callback);
		~MIDIMapping();

		void poll();

		void set_midi_input(uint32_t index) const;
		[[nodiscard]] std::vector<std::string> get_midi_input_names() const;
	private:
		enum MIDIBindType : uint8_t { CC = 0, NOTE = 1 };
		enum InputActionType : uint8_t { FORWARD = 0, SCRIPT_EVENT = 1 };

		typedef struct {
			std::string forward_id;

			uint8_t bind_data;
			MIDIBindType bind_type;

			InputActionType action_type;
		} midi_to_control_binding_t;

		rt::midi::RtMidiIn *m_midi_in;
		sol::state m_lua;

		const param_set_callback_t &m_param_set_callback;
		const param_get_callback_t &m_param_get_callback;

		const cursor_move_by_callback_t &m_cursor_move_by_callback;
		const cursor_move_to_callback_t &m_cursor_move_to_callback;
		const cursor_load_callback_t &m_cursor_load_callback;
		const cursor_back_callback_t &m_cursor_back_callback;

		const note_on_callback_t &m_note_on_callback;
		const note_off_callback_t &m_note_off_callback;

		std::unordered_map<std::string, midi_to_control_binding_t> m_midi_to_control_bindings;
		std::unordered_map<std::string, sol::function> m_control_to_lua_bindings;

		std::array<uint8_t, 128> m_midi_cc_values{};
	};
} // namespace YSKMH
