#include "YourSound/IntegratedPlayers/BasicOSCPlayer.hpp"

#include <imgui.h>

using namespace YourSound::BinPlayer;

static void imgui_basic_oscillator_dropdown(BasicOscillator *osc, const char *label) {
	uint8_t current = *osc;

	static const char *osc_names[5] = {"Square", "Triangle", "Sine", "Sawtooth", "Noise"};

	if (ImGui::BeginCombo(label, osc_names[current])) {
		for (uint16_t i = 0; i < 5; i++) {
			const bool is_selected = i == current;

			if (ImGui::Selectable(osc_names[i], is_selected)) current = i;
			if (is_selected) ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	*osc = static_cast<BasicOscillator>(current);
}

using namespace YourSound::BinPlayer::Integrated;

void BasicOSCPlayer::render(float *output_buffer, const uint16_t number_samples) {
	if (m_note_midi == 0) {
		std::fill_n(output_buffer, number_samples * 2, 0.0f);
		return;
	}

	const float freq = midi_to_freq(m_note_midi, m_pitch_bend);
	const float phase_increment = freq / m_sample_rate;

	for (uint16_t i = 0; i < number_samples; ++i) {
		float sample = calculate_basic_osc(m_osc, m_phase);

		sample = sample * 2.0f - 1.0f;

		output_buffer[i * 2 + 0] = sample;
		output_buffer[i * 2 + 1] = sample;

		m_phase += phase_increment;
		if (m_phase >= 1.0f) m_phase -= 1.0f;
	}

	scale_float_array(output_buffer, number_samples * 2, m_note_velocity);
}

void BasicOSCPlayer::set_parameter(const char *param_id, const float value) {
	if (std::strcmp(param_id, "osc") == 0) m_osc = static_cast<BasicOscillator>(std::roundf(value * 4));
	else if (std::strcmp(param_id, "_pitch_bend")) m_pitch_bend = value * 2.f - 1.f;
}

float BasicOSCPlayer::get_parameter(const char *param_id) {
	if (std::strcmp(param_id, "osc") == 0) return static_cast<uint8_t>(m_osc) / 4.f;
	return 0.f;
}

void BasicOSCPlayer::render_graphics() {
	ImGui::Text("OSC");
	ImGui::SameLine();
	imgui_basic_oscillator_dropdown(&m_osc, "##osc");
}
