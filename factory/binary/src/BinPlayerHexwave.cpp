#include "BinPlayerHexwave.hpp"

#define STB_HEXWAVE_IMPLEMENTATION
#include <YourSound/BinPlayerSTD.hpp>
#include <YourSound/Serialisation.hpp>
#include <YourSound/UI/Controls.hpp>
#include <stb_hexwave.h>

#include <iostream>

BinPlayerHexwave::BinPlayerHexwave() {
	m_monophony_controller.tuning = 440.f;

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
	m_monophony_controller.note_on(midi_note_number, velocity);
}

void BinPlayerHexwave::note_off(uint8_t midi_note_number) { m_monophony_controller.note_off(midi_note_number); }
void BinPlayerHexwave::render(float *output_buffer, const uint16_t number_samples) {
	const YourSound::BinPlayer::MonophonyController::state_t mono_state =
		m_monophony_controller.get_state(static_cast<float>(number_samples) / m_sample_rate);

	if (mono_state.frequency == 0.f) {
		std::fill_n(output_buffer, number_samples * 2, 0.0f);
		return;
	}

	std::vector<float> mono_buffer(number_samples);

	hexwave_generate_samples(mono_buffer.data(), number_samples, m_hexwave, mono_state.frequency / m_sample_rate);
	YourSound::BinPlayer::scale_float_array(mono_buffer.data(), number_samples,
											mono_state.velocity * YourSound::BinPlayer::calculate_amp_envelope(
																	  m_amp_envelope, mono_state.note_time, false));
	YourSound::BinPlayer::mono_to_stereo(mono_buffer.data(), output_buffer, number_samples);
}

uint64_t BinPlayerHexwave::store_calc_size(const bool store_reference) { return 1 + 1 + 4 + 4 + 4 + (4 * 4) + (4 * 6) + 4; }
void BinPlayerHexwave::store(uint8_t *output_buffer, bool store_reference) {
	uint8_t offset = 0;
	output_buffer[offset++] = 255; // to distinguish between old and new patches
	output_buffer[offset++] = m_reflect;

	YourSound::write_float_be<float>(output_buffer + offset, m_peak_time);
	offset += 4;
	YourSound::write_float_be<float>(output_buffer + offset, m_half_height);
	offset += 4;
	YourSound::write_float_be<float>(output_buffer + offset, m_zero_wait);
	offset += 4;

	YourSound::write_float_be<float>(output_buffer + offset, m_amp_envelope.attack_time);
	offset += 4;
	YourSound::write_float_be<float>(output_buffer + offset, m_amp_envelope.decay_time);
	offset += 4;
	YourSound::write_float_be<float>(output_buffer + offset, m_amp_envelope.sustain_volume);
	offset += 4;
	YourSound::write_float_be<float>(output_buffer + offset, m_amp_envelope.release_time);
	offset += 4;

	YourSound::write_float_be<float>(output_buffer + offset, m_filter_envelope.attack_time);
	offset += 4;
	YourSound::write_float_be<float>(output_buffer + offset, m_filter_envelope.decay_time);
	offset += 4;
	YourSound::write_float_be<float>(output_buffer + offset, m_filter_envelope.sustain_level);
	offset += 4;
	YourSound::write_float_be<float>(output_buffer + offset, m_filter_envelope.release_time);
	offset += 4;
	YourSound::write_float_be<float>(output_buffer + offset, m_filter_envelope.amount);
	offset += 4;
	YourSound::write_float_be<float>(output_buffer + offset, m_filter_envelope.base_cutoff);
	offset += 4;

	YourSound::write_float_be<float>(output_buffer + offset, m_monophony_controller.glide_seconds);
	offset += 4;
}

void BinPlayerHexwave::load(const uint8_t *input_buffer) {
	uint8_t offset = 0;
	if (input_buffer[offset] != 255) {
		m_reflect = input_buffer[offset++];
		m_peak_time = YourSound::read_float_be<float>(input_buffer + offset);
		offset += 4;
		m_half_height = YourSound::read_float_be<float>(input_buffer + offset);
		offset += 4;
		m_zero_wait = YourSound::read_float_be<float>(input_buffer + offset);
		offset += 4;
		return;
	}

	++offset;
	m_reflect = input_buffer[offset++];
	m_peak_time = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;
	m_half_height = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;
	m_zero_wait = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;

	m_amp_envelope.attack_time = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;
	m_amp_envelope.decay_time = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;
	m_amp_envelope.sustain_volume = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;
	m_amp_envelope.release_time = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;

	m_filter_envelope.attack_time = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;
	m_filter_envelope.decay_time = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;
	m_filter_envelope.sustain_level = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;
	m_filter_envelope.release_time = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;
	m_filter_envelope.amount = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;
	m_filter_envelope.base_cutoff = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;

	m_monophony_controller.glide_seconds = YourSound::read_float_be<float>(input_buffer + offset);
	offset += 4;
}

void BinPlayerHexwave::set_sample_rate(const uint32_t value) { m_sample_rate = value; }
void BinPlayerHexwave::set_parameter(const char *param_id, const float value) {
	if (std::strcmp(param_id, "reflect") == 0 || std::strcmp(param_id, "_qparam_buttonlike_toggle_1") == 0) {
		if (value >= 0.5) m_reflect = true;
		else m_reflect = false;
		p_update_params();
	} else if (std::strcmp(param_id, "peak_time") == 0 || std::strcmp(param_id, "_qparam_wheellike_1") == 0) {
		m_peak_time = value;
		p_update_params();
	} else if (std::strcmp(param_id, "half_height") == 0 || std::strcmp(param_id, "_qparam_wheellike_2") == 0) {
		m_half_height = value * 2.f - 1.f;
		p_update_params();
	} else if (std::strcmp(param_id, "zero_wait") == 0 || std::strcmp(param_id, "_qparam_wheellike_3") == 0) {
		m_zero_wait = value;
		p_update_params();
	} else if (std::strcmp(param_id, "_pitch_bend") == 0) m_monophony_controller.pitch_bend = value * 2.f - 1.f;
	else if (std::strcmp(param_id, "_midi_cc_64") == 0) {
		if (value >= 0.5f) m_monophony_controller.sustain_on();
		else m_monophony_controller.sustain_off();
	} else if (std::strcmp(param_id, "glide") == 0 || std::strcmp(param_id, "_qparam_faderlike_1") == 0)
		m_monophony_controller.glide_seconds = value * 10.f;
}

float BinPlayerHexwave::get_parameter(const char *param_id) {
	if (std::strcmp(param_id, "reflect") == 0 || std::strcmp(param_id, "_qparam_buttonlike_toggle_1") == 0)
		return m_reflect ? 1.f : 0.f;
	if (std::strcmp(param_id, "peak_time") == 0 || std::strcmp(param_id, "_qparam_wheellike_1") == 0)
		return m_peak_time;
	if (std::strcmp(param_id, "half_height") == 0 || std::strcmp(param_id, "_qparam_wheellike_2") == 0)
		return (m_half_height + 1.f) * 0.5f;
	if (std::strcmp(param_id, "zero_wait") == 0 || std::strcmp(param_id, "_qparam_wheellike_3") == 0)
		return m_zero_wait;
	if (std::strcmp(param_id, "glide") == 0 || std::strcmp(param_id, "_qparam_faderlike_1") == 0)
		return m_monophony_controller.glide_seconds / 10.f;
	return 0.f;
}

void BinPlayerHexwave::get_parameters(const char **buffer) {
	buffer[0] = "reflect";
	buffer[1] = "peak_time";
	buffer[2] = "half_height";
	buffer[3] = "zero_wait";
}

void BinPlayerHexwave::reset() { m_monophony_controller.reset(); }

void BinPlayerHexwave::render_graphics() {
	ysbp_ui_align_cursor_for_center(m_top_row_width);

	ysbp_ui_begin_group();
	ysbp_ui_checkbox("Reflect", &m_reflect);
	ysbp_ui_end_group();

	if (const float current_width = ysbp_ui_get_item_rect_size().x; current_width > 0.0f)
		m_top_row_width = current_width;
	ysbp_ui_align_cursor_for_center(m_knob_row_width);

	ysbp_ui_begin_group();
	ysbp_ui_knob("Peak Time", &m_peak_time, 0.f, 1.f, 0.01f, "%.2f", ysbp_ui_ImGuiKnobVariant_Wiper, 75);
	ysbp_ui_same_line();
	ysbp_ui_knob("Half Height", &m_half_height, -1.f, 1.f, 0.01f, "%.2f", ysbp_ui_ImGuiKnobVariant_Wiper, 75);
	ysbp_ui_same_line();
	ysbp_ui_knob("Zero Wait", &m_zero_wait, 0.f, 1.f, 0.01f, "%.2f", ysbp_ui_ImGuiKnobVariant_Wiper, 75);
	ysbp_ui_end_group();

	if (const float current_width = ysbp_ui_get_item_rect_size().x; current_width > 0.0f)
		m_knob_row_width = current_width;

	ysbp_ui_spacing();

	ysbp_ui_align_cursor_for_center(m_envelope_row_width);
	ysbp_ui_begin_group();

	ysbp_ui_begin_group();
	ysbp_ui_text("Amplitude");

	YourSound::BinPlayer::amp_envelope_edit_ui(m_amp_envelope);
	ysbp_ui_end_group();
	if (const float current_height = ysbp_ui_get_item_rect_size().y; current_height > 0.0f)
		m_envelope_row_height = current_height;

	ysbp_ui_same_line();

	ysbp_ui_text("Filter");
	ysbp_ui_end_group();

	ysbp_ui_same_line();

	ysbp_ui_begin_group();
	ysbp_ui_float_slider_vertical("##hxwav_glide", {ysbp_ui_get_frame_height(), m_envelope_row_height},
								  &m_monophony_controller.glide_seconds, 0.f, 10.f, "",
								  ysbp_ui_ImGuiSliderFlags_Logarithmic | ysbp_ui_ImGuiSliderFlags_AlwaysClamp);
	ysbp_ui_same_line();
	ysbp_ui_begin_group();
	ysbp_ui_text("Glide");
	ysbp_ui_push_font(ysbp_ui_FONT_MONO);
	ysbp_ui_text(std::format("{:04.2f}s", m_monophony_controller.glide_seconds).c_str());
	ysbp_ui_pop_font();
	ysbp_ui_end_group();

	ysbp_ui_end_group();
	if (const float current_width = ysbp_ui_get_item_rect_size().x; current_width > 0.0f)
		m_envelope_row_width = current_width;

	const auto [note_time, velocity, frequency] = m_monophony_controller.get_state(0.f);
	ysbp_ui_text(
		std::format("F: {}\nV: {}\nT: {}\nG: {}", frequency, velocity, note_time, m_monophony_controller.glide_seconds)
			.c_str());

	p_update_params();
}

void BinPlayerHexwave::p_update_params() const {
	hexwave_change(m_hexwave, m_reflect, m_peak_time, m_half_height, m_zero_wait);
}
