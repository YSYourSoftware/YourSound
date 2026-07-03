#include "YourSound/IntegratedPlayers/LuaPlayers.hpp"
#include "YourSound/BinPlayerInterface.hpp"
#include "YourSound/UI/Colours.hpp"
#include "YourSound/UI/Fonts.hpp"

#include <TextEditor.h>
#include <imgui.h>

#include <span>

using namespace YourSound::BinPlayer;

Integrated::BinPlayerLua::BinPlayerLua() {
	m_lua_state.open_libraries(sol::lib::base, sol::lib::bit32, sol::lib::coroutine, sol::lib::ffi, sol::lib::jit,
							   sol::lib::os, sol::lib::io, sol::lib::string, sol::lib::table, sol::lib::utf8);
	p_emit("init");

	m_text_editor = new TextEditor();

	m_text_editor->SetText(
		"-- YourSound Lua Player Demo: Sine Wave\n\nlocal pitchBend = 0.0\nlocal phase = "
		"0.0\n\nplayer:bindParam(\"_pitch_wheel\", "
		"function(value)\n\tpitchBend = (value * 2.0) - 1.0\nend)\n\nplayer:bind(\"renderUI\", "
		"function()\n\tui:text(\"Hello, Lua!\")\nend)\n\nplayer:bind(\"render\", function(samples)\n\t\nend))");
	m_text_editor->SetLanguage(TextEditor::Language::Lua());
	m_text_editor->SetAutoIndentEnabled(true);
	m_text_editor->SetShowMiniMapEnabled(true);
	m_text_editor->SetShowScrollbarMiniMapEnabled(true);

	TextEditor::AutoCompleteConfig auto_complete_config;
	auto_complete_config.callback = [this](TextEditor::AutoCompleteState &state) {
		// TODO: move to LuaLS
		if (!state.inIdentifier) return;

		state.suggestions.clear();

		constexpr std::array<std::string_view, 2> PLAYER_COMPLETE_TERMS = {"bind", "bindParam"};
		constexpr std::array<std::string_view, 2> UI_COMPLETE_TERMS = {"text", "button"};

		constexpr std::array<std::string_view, 2> TOP_LEVEL_COMPLETE_TERMS = {"player", "ui"};
		const std::array<std::span<const std::string_view>, 2> SECOND_LEVEL_COMPLETE_TERMS = {PLAYER_COMPLETE_TERMS,
																							  UI_COMPLETE_TERMS};

		TextEditor::DocPos before_search_term = state.searchTermStart;
		before_search_term.index -= 1;

		if (const bool second_level = m_text_editor->GetSectionText(before_search_term, state.searchTermStart) == ":";
			!second_level) {
			state.suggestions.reserve(TOP_LEVEL_COMPLETE_TERMS.size());
			for (const auto &suggestion : TOP_LEVEL_COMPLETE_TERMS) state.suggestions.emplace_back(suggestion);
		} // else {}
	};

	m_text_editor->SetAutoCompleteConfig(&auto_complete_config);
}

Integrated::BinPlayerLua::~BinPlayerLua() {
	p_emit("destroy");
	delete m_text_editor;
}

void Integrated::BinPlayerLua::note_on(uint8_t midi_note_number, float velocity) {
	p_emit("noteOn", midi_note_number, velocity);
}

void Integrated::BinPlayerLua::note_off(uint8_t midi_note_number) { p_emit("noteOff", midi_note_number); }

void Integrated::BinPlayerLua::render(float *output_buffer, uint16_t number_samples) {
	if (!m_lua_subscribers.contains("render")) {
		std::fill_n(output_buffer, number_samples * 2, 0.0f);
		return;
	}

	m_sample_write_pos = output_buffer;
	m_samples_written = 0;
	m_sample_write_target = number_samples;
	p_emit("render", number_samples);

	if (m_samples_written != m_sample_write_target)
		p_exception(
			std::format("UnderflowError: expected {} sample pairs, got {}", m_sample_write_target, m_samples_written));
}

uint64_t Integrated::BinPlayerLua::store_calc_size(const bool store_reference) {
	m_store_data_pos = nullptr;
	m_store_data_size = 0;
	p_emit("store", store_reference);
	return m_store_data_size;
}

void Integrated::BinPlayerLua::store(uint8_t *output_buffer, const bool store_reference) {
	m_store_data_pos = output_buffer;
	m_target_data_size = m_store_data_size;
	m_store_data_size = 0;

	p_emit("store", store_reference);

	if (m_store_data_size != m_target_data_size)
		p_exception(std::format("UnderflowError: expected {} bytes, got {}", m_target_data_size, m_store_data_size));
}

void Integrated::BinPlayerLua::load(const uint8_t *input_buffer) {
	m_load_data = input_buffer;
	p_emit("load");
}

void Integrated::BinPlayerLua::set_bpm(uint16_t value) { p_emit("setBPM", value); }
void Integrated::BinPlayerLua::set_sample_rate(uint32_t value) { p_emit("setSampleRate", value); }
void Integrated::BinPlayerLua::set_parameter(const char *param_id, const float value) {
	p_emit_param(std::string(param_id), value);
}

uint8_t Integrated::BinPlayerLua::get_parameter_count() {
	m_parameters.clear();
	p_emit("getParameters");
	return m_parameters.size();
}

void Integrated::BinPlayerLua::get_parameters(const char **buffer) {
	uint8_t i = 0;
	for (const auto &id : m_parameters) {
		buffer[i] = id.c_str();
		i++;
	}
}

void Integrated::BinPlayerLua::render_graphics() {
	if (!m_release_mode) {
		ImGui::BeginTabBar("bpl_main_tab");

		if (ImGui::BeginTabItem("Release UI")) {
			p_emit("renderUI");
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Script")) {
			ImGui::PushFont(UI::g_imgui_mono_font);
			m_text_editor->Render("Script", {ImGui::GetContentRegionAvail().x, 500});
			ImGui::PopFont();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(36.f / 255.f, 36.f / 255.f, 36.f / 255.f, 1.f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.5f, 2.5f));
			ImGui::BeginChild("Logs", {ImGui::GetContentRegionAvail().x, 100}, ImGuiChildFlags_Borders,
							  ImGuiWindowFlags_NoDecoration);

			ImGui::PushFont(UI::g_imgui_mono_font);
			for (const auto &message : m_log_messages) {
				switch (message.severity) {
				case LogMessage::INFO: ImGui::PushStyleColor(ImGuiCol_Text, UI::Colours::BUTTON_BLUE_ACTIVE); break;
				case LogMessage::WARNING:
					ImGui::PushStyleColor(ImGuiCol_Text, UI::Colours::BUTTON_YELLOW_ACTIVE);
					break;
				case LogMessage::ERROR:
				case LogMessage::EXCEPTION: ImGui::PushStyleColor(ImGuiCol_Text, UI::Colours::BUTTON_RED_ACTIVE); break;
				}

				ImGui::Text(message.message.c_str());
				ImGui::PopStyleColor();
			}
			ImGui::PopFont();

			ImGui::EndChild();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Docs")) { ImGui::EndTabItem(); }

		ImGui::EndTabBar();
	} else p_emit("renderUI");
}

void Integrated::BinPlayerLua::reset() { p_emit("reset"); }

void Integrated::BinPlayerLua::p_log(const std::string &message) {
	m_log_messages.push_back({.message = message, .severity = LogMessage::INFO});
}

void Integrated::BinPlayerLua::p_log_warning(const std::string &message) {
	m_log_messages.push_back({.message = message, .severity = LogMessage::WARNING});
}

void Integrated::BinPlayerLua::p_log_error(const std::string &message) {
	m_log_messages.push_back({.message = message, .severity = LogMessage::ERROR});
}

void Integrated::BinPlayerLua::p_exception(const std::string &message) {
	lua_Debug debug;

	if (lua_getstack(m_lua_state.lua_state(), 0, &debug) != 1) {
		ysbp_show_error(get_error_source(),
						std::format("Exception in lua script (failed to get line info): {}", message).c_str());
		m_log_messages.push_back(
			{.message = std::format("Exception in lua script (failed to get line info): {}", message),
			 .severity = LogMessage::EXCEPTION});
		return;
	}

	lua_getinfo(m_lua_state.lua_state(), "ln", &debug);

	m_exception_line = debug.currentline;

	ysbp_show_error(get_error_source(), std::format("Exception in script (line {}, in function {}): {}",
													m_exception_line, debug.name, message)
											.c_str());
	m_log_messages.push_back({.message = std::format("Exception in script (line {}, in function {}): {}",
													 m_exception_line, debug.name, message),
							  .severity = LogMessage::EXCEPTION});
	m_text_editor->ScrollToLine(m_exception_line, TextEditor::Scroll::alignMiddle);
	m_text_editor->AddMarker(m_exception_line, 0, IM_COL32(128, 0, 32, 128), "", message);
}

void Integrated::BinPlayerLua::p_write_samples(const float l_sample, const float r_sample) {
	m_samples_written++;
	if (m_samples_written > m_sample_write_target) {
		p_exception(
			std::format("OverflowError: expected {} sample pairs, got {}", m_sample_write_target, m_samples_written));
		return;
	}

	*(m_sample_write_pos++) = l_sample;
	*(m_sample_write_pos++) = r_sample;
}

void Integrated::BinPlayerLua::p_store_string(const std::string &string) {
	m_store_data_size += string.size() + 1;
	if (m_store_data_size > m_target_data_size) {
		p_exception(std::format("OverflowError: expected {} bytes, got {}", m_target_data_size, m_store_data_size));
		return;
	}

	std::memcpy(m_store_data_pos, string.data(), string.size() + 1);
	m_store_data_pos += string.size() + 1;
}

void Integrated::BinPlayerLua::p_store_number(lua_Number number) {
	m_store_data_size += 8;
	if (m_store_data_size > m_target_data_size) {
		p_exception(std::format("OverflowError: expected {} bytes, got {}", m_target_data_size, m_store_data_size));
		return;
	}

	std::memcpy(m_store_data_pos, reinterpret_cast<double *>(&number), 8);
	m_store_data_pos += 8;
}

std::string Integrated::BinPlayerLua::p_load_string() {
	std::string result{reinterpret_cast<const char *>(m_load_data)};
	m_load_data += result.size() + 1;
	return result;
}

lua_Number Integrated::BinPlayerLua::p_load_number() {
	double number;

	std::memcpy(&number, m_load_data, 8);
	m_load_data += 8;

	return static_cast<lua_Number>(number);
}

void Integrated::BinPlayerLua::p_add_parameter(const std::string &id) {
	m_parameters.push_back(id);
	if (m_parameters.size() > UINT8_MAX) p_exception(std::format("OverflowError: max {} parameters", UINT8_MAX));
}

void Integrated::BinPlayerLua::p_emit_param(const std::string &param, const float value) const {
	if (const auto it = m_lua_param_subscribers.find(param); it != m_lua_param_subscribers.end()) it->second(value);
}
