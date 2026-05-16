#include "BinPlayerHexwave.hpp"

#include "Utils.hpp"

#include <imgui-knobs.h>
#include <imgui.h>
#define STB_HEXWAVE_IMPLEMENTATION
#include <iostream>
#include <stb_hexwave.h>
#include <YourSound/BinPlayerSTD.hpp>
#include <YourSound/Serialisation.hpp>

BinPlayerHexwave::BinPlayerHexwave() {
	m_hexwave_user_buffer = new float[16 * 32 * (16 + 1)];
	hexwave_init(32, 16, m_hexwave_user_buffer);

	m_hexwave = new HexWave();
	hexwave_create(m_hexwave, m_reflect, m_peak_time, m_half_height, m_zero_wait);
}

BinPlayerHexwave::~BinPlayerHexwave() {
	hexwave_shutdown(m_hexwave_user_buffer);
	delete[] m_hexwave_user_buffer;
}

void BinPlayerHexwave::note_on(const uint8_t midi_note_number, const float velocity) {
	m_note_on = midi_note_number;
	m_note_velocity = velocity;
}

void BinPlayerHexwave::note_off(uint8_t midi_note_number) {m_note_on = 0;}
void BinPlayerHexwave::render(float *output_buffer, const uint16_t number_samples) {
	if (m_note_on == 0) {
		std::fill_n(output_buffer, number_samples * 2, 0.0f);
		return;
	}

	auto *mono_buffer = new float[number_samples];

	hexwave_generate_samples(mono_buffer, number_samples, m_hexwave, YourSound::BinPlayer::midi_to_freq(m_note_on, m_pitch_bend) / m_sample_rate);
	YourSound::BinPlayer::scale_float_array(mono_buffer, number_samples * 2, m_note_velocity);
	YourSound::BinPlayer::mono_to_stereo(mono_buffer, output_buffer, number_samples);

	delete[] mono_buffer;
}

uint64_t BinPlayerHexwave::store_calc_size(bool store_reference) const {return 1 + 4 + 4 + 4;}
void BinPlayerHexwave::store(uint8_t *output_buffer, bool store_reference) const {
	output_buffer[0] = m_reflect;
	YourSound::write_float_be<float>(output_buffer + 1, m_peak_time);
	YourSound::write_float_be<float>(output_buffer + 5, m_half_height);
	YourSound::write_float_be<float>(output_buffer + 9, m_zero_wait);
}

void BinPlayerHexwave::load(const uint8_t *input_buffer) {
	uint8_t offset = 0;
	m_reflect = input_buffer[offset++];
	m_peak_time = YourSound::read_float_be<float>(input_buffer + offset); offset += 4;
	m_half_height = YourSound::read_float_be<float>(input_buffer + offset); offset += 4;
	m_zero_wait = YourSound::read_float_be<float>(input_buffer + offset); offset += 4;
}

void BinPlayerHexwave::set_sample_rate(const uint32_t value) {m_sample_rate = value;}
void BinPlayerHexwave::set_parameter(const char *param_id, const float value) {
	if (std::strcmp(param_id, "reflect") == 0) {if (value >= 0.5) m_reflect = true; else m_reflect = false; p_update_params();}
	else if (std::strcmp(param_id, "peak_time") == 0) {m_peak_time = value; p_update_params();}
	else if (std::strcmp(param_id, "half_height") == 0) {m_half_height = value * 2.f - 1.f; p_update_params();}
	else if (std::strcmp(param_id, "zero_wait") == 0) {m_zero_wait = value; p_update_params();}

	else if (std::strcmp(param_id, "_pitch_bend") == 0) m_pitch_bend = value * 2.f - 1.f;
}

void BinPlayerHexwave::get_parameters(const char **buffer) const {
	buffer[0] = "reflect";
	buffer[1] = "peak_time";
	buffer[2] = "half_height";
	buffer[3] = "zero_wait";
}

void BinPlayerHexwave::reset() {m_note_on = 0;}

void BinPlayerHexwave::render_graphics(YS_ImContextHandle im_context) {
	ImGui::SetCurrentContext(static_cast<ImGuiContext*>(im_context));

	imgui_align_cursor_for_center(m_top_row_width);

	ImGui::BeginGroup();
	ImGui::Checkbox("Reflect", &m_reflect);
	ImGui::EndGroup();

	if (const float current_width = ImGui::GetItemRectSize().x; current_width > 0.0f) m_top_row_width = current_width;
	imgui_align_cursor_for_center(m_knob_row_width);

	ImGui::BeginGroup();
	ImGuiKnobs::Knob("Peak Time", &m_peak_time, 0.f, 1.f, 0.01f, "%.2f", ImGuiKnobVariant_Wiper, 75);
	ImGui::SameLine();
	ImGuiKnobs::Knob("Half Height", &m_half_height, -1.f, 1.f, 0.01f, "%.2f", ImGuiKnobVariant_Wiper, 75);
	ImGui::SameLine();
	ImGuiKnobs::Knob("Zero Wait", &m_zero_wait, 0.f, 1.f, 0.01f, "%.2f", ImGuiKnobVariant_Wiper, 75);
	ImGui::EndGroup();

	if (const float current_width = ImGui::GetItemRectSize().x; current_width > 0.0f) m_knob_row_width = current_width;

	p_update_params();
}

void BinPlayerHexwave::p_update_params() const {hexwave_change(m_hexwave, m_reflect, m_peak_time, m_half_height, m_zero_wait);}