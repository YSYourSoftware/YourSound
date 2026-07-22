#include "BinPlayerWavetable.hpp"

#include <YourSound/Serialisation.hpp>
#include <YourSound/UI/Controls.hpp>

void BinPlayerWavetable::note_on(const uint8_t midi_note_number, const float velocity) {
	m_polyphony_controller.note_on(midi_note_number, velocity);
}

void BinPlayerWavetable::note_off(const uint8_t midi_note_number) { m_polyphony_controller.note_off(midi_note_number); }

void BinPlayerWavetable::render(float *output_buffer, const uint16_t number_samples) {
	std::fill_n(output_buffer, number_samples * 2, 0.f);
}

uint64_t BinPlayerWavetable::store_calc_size(const bool store_reference) {
	return (WAVE_SAMPLE_COUNT * WAVE_COUNT * 2) + (1) + (4 + 4 + 4 + 4) + (4);
}

void BinPlayerWavetable::store(uint8_t *output_buffer, const bool store_reference) {
	std::memcpy(output_buffer, m_wavetable.data(), WAVE_SAMPLE_COUNT * WAVE_COUNT * 2);
	output_buffer += WAVE_SAMPLE_COUNT * WAVE_COUNT * 2;

	*(output_buffer++) = m_manual_morph;

	YourSound::write_float_be(output_buffer, m_amp_envelope.attack_time);
	output_buffer += 4;
	YourSound::write_float_be(output_buffer, m_amp_envelope.decay_time);
	output_buffer += 4;
	YourSound::write_float_be(output_buffer, m_amp_envelope.sustain_volume);
	output_buffer += 4;
	YourSound::write_float_be(output_buffer, m_amp_envelope.release_time);
	output_buffer += 4;

	YourSound::write_float_be(output_buffer, m_osc_rate);
	output_buffer += 4;
}

void BinPlayerWavetable::load(const uint8_t *input_buffer) {
	std::memcpy(m_wavetable.data(), input_buffer, WAVE_SAMPLE_COUNT * WAVE_COUNT * 2);
	input_buffer += WAVE_SAMPLE_COUNT * WAVE_COUNT * 2;

	m_manual_morph = *(input_buffer++);

	m_amp_envelope.attack_time = YourSound::read_float_be<float>(input_buffer);
	input_buffer += 4;
	m_amp_envelope.decay_time = YourSound::read_float_be<float>(input_buffer);
	input_buffer += 4;
	m_amp_envelope.sustain_volume = YourSound::read_float_be<float>(input_buffer);
	input_buffer += 4;
	m_amp_envelope.release_time = YourSound::read_float_be<float>(input_buffer);
	input_buffer += 4;

	m_osc_rate = YourSound::read_float_be<float>(input_buffer);
	input_buffer += 4;
}

void BinPlayerWavetable::set_parameter(const char *param_id, const float value) {
	if (std::strcmp(param_id, "_pitch_wheel") == 0) {
		m_polyphony_controller.pitch_bend_update((value * 2.f) - 1.f);
		return;
	}

	if (std::strcmp(param_id, "_midi_cc_64") == 0) {
		if (value >= 0.5f) m_polyphony_controller.sustain_on();
		else m_polyphony_controller.sustain_off();
		return;
	}

	if (std::strcmp(param_id, "_qparam_buttonlike_toggle_1") == 0 || std::strcmp(param_id, "manual_morph") == 0) {
		m_manual_morph = value >= 0.5f;
		return;
	}

	if (std::strcmp(param_id, "_midi_cc_1") == 0) {
		m_selected_wave = static_cast<uint16_t>(value * (WAVE_COUNT - 1));
		return;
	}

	if (std::strcmp(param_id, "_qparam_wheellike_1") == 0 || std::strcmp(param_id, "amp_env_attack_time") == 0) {
		m_amp_envelope.attack_time = value * 10.f;
		return;
	}

	if (std::strcmp(param_id, "_qparam_wheellike_2") == 0 || std::strcmp(param_id, "amp_env_decay_time") == 0) {
		m_amp_envelope.decay_time = value * 10.f;
		return;
	}

	if (std::strcmp(param_id, "_qparam_wheellike_3") == 0 || std::strcmp(param_id, "amp_env_sustain_vol") == 0) {
		m_amp_envelope.sustain_volume = value;
		return;
	}

	if (std::strcmp(param_id, "_qparam_wheellike_4") == 0 || std::strcmp(param_id, "amp_env_release_time") == 0) {
		m_amp_envelope.release_time = value * 10.f;
		return;
	}

	if (std::strcmp(param_id, "_qparam_wheellike_5") == 0 || std::strcmp(param_id, "osc_rate") == 0) {
		m_osc_rate = value * 2.f;
		return;
	}
}

float BinPlayerWavetable::get_parameter(const char *param_id) {
	if (std::strcmp(param_id, "_qparam_buttonlike_toggle_1") == 0 || std::strcmp(param_id, "manual_morph") == 0)
		return m_manual_morph ? 1.f : 0.f;
	if (std::strcmp(param_id, "_midi_cc_1") == 0) return m_selected_wave / (WAVE_COUNT - 1.f);
	if (std::strcmp(param_id, "_qparam_wheellike_1") == 0 || std::strcmp(param_id, "amp_env_attack_time") == 0)
		return m_amp_envelope.attack_time * 0.1f;
	if (std::strcmp(param_id, "_qparam_wheellike_2") == 0 || std::strcmp(param_id, "amp_env_decay_time") == 0)
		return m_amp_envelope.decay_time * 0.1f;
	if (std::strcmp(param_id, "_qparam_wheellike_3") == 0 || std::strcmp(param_id, "amp_env_sustain_vol") == 0)
		return m_amp_envelope.sustain_volume;
	if (std::strcmp(param_id, "_qparam_wheellike_4") == 0 || std::strcmp(param_id, "amp_env_release_time") == 0)
		return m_amp_envelope.release_time * 0.1f;
	if (std::strcmp(param_id, "_qparam_wheellike_5") == 0 || std::strcmp(param_id, "osc_rate") == 0)
		return m_osc_rate * 0.5f;
	return 0.f;
}

void BinPlayerWavetable::get_parameters(const char **buffer) {
	buffer[0] = "manual_morph";
	buffer[1] = "amp_env_attack_time";
	buffer[2] = "amp_env_decay_time";
	buffer[3] = "amp_env_sustain_vol";
	buffer[4] = "amp_env_release_time";
	buffer[5] = "osc_rate";
}

void BinPlayerWavetable::render_graphics() {
	ysbp_ui_align_cursor_for_center(m_wavetable_view_width);
	ysbp_ui_begin_group();
	ysbp_ui_line_graph("",
					   [](void *data, const int idx) -> float {
						   const auto *samples = static_cast<uint16_t *>(data);
						   return (samples[idx] / static_cast<float>(UINT16_MAX) * 2.f) - 1.f;
					   },
					   m_wavetable.data() + (WAVE_SAMPLE_COUNT * m_selected_wave), WAVE_SAMPLE_COUNT, 0, "", -1.f, 1.f,
					   {500, 200});

	if (m_manual_morph) {
		int32_t manual_morph_u32 = m_selected_wave;
		ysbp_ui_set_next_item_width(500.f);
		if (ysbp_ui_int_slider("", &manual_morph_u32, 0, WAVE_COUNT - 1, "%d", ysbp_ui_ImGuiSliderFlags_AlwaysClamp))
			m_selected_wave = manual_morph_u32;
	}
	ysbp_ui_end_group();

	if (const float current_width = ysbp_ui_get_item_rect_size().x; current_width > 0.0f)
		m_wavetable_view_width = current_width;

	ysbp_ui_align_cursor_for_center(m_checkbox_width);
	ysbp_ui_begin_group();
	ysbp_ui_checkbox("Manual Morph", &m_manual_morph);
	ysbp_ui_end_group();
	if (const float current_width = ysbp_ui_get_item_rect_size().x; current_width > 0.0f)
		m_checkbox_width = current_width;
}

void BinPlayerWavetable::reset() { m_polyphony_controller.reset(); }
