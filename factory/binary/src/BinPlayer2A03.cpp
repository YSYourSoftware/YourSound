#include "BinPlayer2A03.hpp"

#include <YourSound/BinPlayerSTD.hpp>
#include <YourSound/UI/Controls.hpp>

static constexpr uint8_t TRIANGLE_TABLE[32] = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5,  4,  3,  2,  1,  0,
											   0,  1,  2,  3,  4,  5,  6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
static constexpr uint8_t DUTY_TABLE[4] = {
	// 12.5%
	0b01000000,

	// 25%
	0b01100000,

	// 50%
	0b01111000,

	// 75%
	0b10011111};

static constexpr uint16_t NOISE_PERIOD_TABLE[16] = {4,	 8,	  16,  32,	64,	 96,   128,	 160,
													202, 254, 380, 508, 762, 1016, 2034, 4068};

static constexpr float CPU_CLOCK = 1789773.f;

void BinPlayer2A03::note_on(const uint8_t midi_note_number, float velocity) {
	m_frequency = YourSound::BinPlayer::midi_to_freq(midi_note_number);
	m_noise_period = 15 - std::min<uint8_t>(midi_note_number % 16, 15);
	m_noise_timer = NOISE_PERIOD_TABLE[m_noise_period];
	m_volume = velocity;
}

void BinPlayer2A03::note_off(uint8_t midi_note_number) { m_frequency = NAN; }

void BinPlayer2A03::render(float *output_buffer, const uint16_t number_samples) {
	if (std::isnan(m_frequency)) {
		std::fill_n(output_buffer, number_samples * 2, 0.f);
		return;
	}

	const float phase_increment = m_frequency / m_sample_rate;
	const float noise_timer_decrement = CPU_CLOCK / m_sample_rate;

	// NOTE: 2a03 can only output samples as nibbles (4-bit, 0-15)
	uint8_t sample = 0;
	for (uint16_t i = 0; i < number_samples; i++) {
		switch (m_waveform) {
		case PULSE:
			sample = (DUTY_TABLE[m_duty_cycle] >> (7 - std::min<uint8_t>(static_cast<uint8_t>(m_phase * 8.0f), 7))) & 1
						 ? 15
						 : 0;
			break;
		case TRIANGLE: sample = TRIANGLE_TABLE[static_cast<uint8_t>(m_phase * 32.f)]; break;
		case NOISE: sample = (m_lfsr & 1) ? 0 : 15; break;
		}

		const float f_sample = sample / 15.f * 2.f - 1.f;
		output_buffer[i * 2 + 0] = f_sample;
		output_buffer[i * 2 + 1] = f_sample;

		if (m_waveform == NOISE) {
			m_noise_timer -= noise_timer_decrement;

			while (m_noise_timer <= 0.f) {
				m_noise_timer += NOISE_PERIOD_TABLE[m_noise_period];

				const uint8_t feedback = ((m_lfsr & 1) ^ ((m_lfsr >> (m_noise_short_period ? 6 : 1)) & 1));
				m_lfsr >>= 1;
				m_lfsr |= feedback << 14;
			}
		} else {
			m_phase += phase_increment;
			while (m_phase >= 1.f) m_phase -= 1.f;
		}
	}

	YourSound::BinPlayer::scale_float_array(output_buffer, number_samples * 2, m_volume);
}

void BinPlayer2A03::store(uint8_t *output_buffer, bool store_reference) {
	*(output_buffer++) = m_waveform;
	*(output_buffer++) = m_duty_cycle;
	*(output_buffer++) = m_noise_short_period;
}

void BinPlayer2A03::load(const uint8_t *input_buffer) {
	m_waveform = static_cast<Waveform>(*(input_buffer++));
	m_duty_cycle = static_cast<DutyCycle>(*(input_buffer++));
	m_noise_short_period = *(input_buffer++);
}

void BinPlayer2A03::set_parameter(const char *param_id, const float value) {
	if (std::strcmp(param_id, "duty_cycle") == 0) m_duty_cycle = static_cast<DutyCycle>(std::roundf(value * 3.f));
}

float BinPlayer2A03::get_parameter(const char *param_id) {
	if (std::strcmp(param_id, "duty_cycle") == 0) return static_cast<uint8_t>(m_duty_cycle) / 3.f;
	return 0.f;
}

void BinPlayer2A03::get_parameters(const char **buffer) { buffer[0] = "duty_cycle"; }

void BinPlayer2A03::render_graphics() {
	const char *waveform_names[3] = {"Pulse (Square)", "Triangle", "Noise"};
	const char *duty_cycle_names[4] = {"12.5%", "25%", "50%", "75% (25% negated)"};

	if (ysbp_ui_begin_combo("Waveform", waveform_names[m_waveform])) {
		for (uint16_t i = 0; i < 3; i++) {
			const bool is_selected = i == m_waveform;

			if (ysbp_ui_combo_selectable(waveform_names[i], is_selected)) m_waveform = static_cast<Waveform>(i);
			if (is_selected) ysbp_ui_combo_set_default_item_focus();
		}

		ysbp_ui_end_combo();
	}

	if (m_waveform == PULSE) {
		if (ysbp_ui_begin_combo("Duty Cycle", duty_cycle_names[m_duty_cycle])) {
			for (uint16_t i = 0; i < 4; i++) {
				const bool is_selected = i == m_duty_cycle;

				if (ysbp_ui_combo_selectable(duty_cycle_names[i], is_selected))
					m_duty_cycle = static_cast<DutyCycle>(i);
				if (is_selected) ysbp_ui_combo_set_default_item_focus();
			}

			ysbp_ui_end_combo();
		}
	} else if (m_waveform == NOISE) {
		ysbp_ui_checkbox("Short Period", &m_noise_short_period);
	}
}
