#include "BinPlayerDebug.hpp"

#include <imgui.h>

BinPlayerDebug::~BinPlayerDebug() { delete m_player; }

void BinPlayerDebug::note_on(const uint8_t midi_note_number, const float velocity) {
	m_notes_on++;
	m_player->note_on(midi_note_number, velocity);
}

void BinPlayerDebug::note_off(const uint8_t midi_note_number) {
	m_notes_on--;
	m_player->note_off(midi_note_number);
}

void BinPlayerDebug::set_bpm(const uint16_t value) {
	m_bpm = value;
	m_player->set_bpm(value);
}

void BinPlayerDebug::set_sample_rate(const uint32_t value) {
	m_sample_rate = value;
	m_player->set_sample_rate(value);
}

void BinPlayerDebug::set_parameter(const char *param_id, const float value) {
	m_parameters[param_id] = value;
	m_player->set_parameter(param_id, value);
}

void BinPlayerDebug::render_graphics() {
	m_player->render_graphics();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("%ubpm, %uHz, %un\nPlayer: %p", m_bpm, m_sample_rate, m_notes_on, m_player);

	for (const auto &[id, value] : m_parameters) {
		ImGui::Text("%s = %f", id.c_str(), value);
	}
}

void BinPlayerDebug::reset() {
	m_parameters.clear();
	m_notes_on = 0;
	m_player->reset();
}
