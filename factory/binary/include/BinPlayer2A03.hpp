#pragma once

#include <YourSound/Player.hpp>

/* TODO:
 * Implement DPCM channel
 * Make the 2A03 player be multi-channel and implement mixer:
PulseOut = 95.88 / ((8128 / (p1 + p2)) + 100)

TNDOut = 159.79 / ((1 / (triangle/8227 + noise/12241 + dmc/22638)) + 100)

Output = PulseOut + TNDOut
 */

class BinPlayer2A03 : public YourSound::Player {
public:
	void note_on(uint8_t midi_note_number, float velocity) override;
	void note_off(uint8_t midi_note_number) override;

	void render(float *output_buffer, uint16_t number_samples) override;

	uint64_t store_calc_size(bool store_reference) override { return 1 + 1 + 1; }
	void store(uint8_t *output_buffer, bool store_reference) override;
	void load(const uint8_t *input_buffer) override;

	void set_bpm(uint16_t value) override {}
	void set_sample_rate(const uint32_t value) override { m_sample_rate = value; }

	void set_parameter(const char *param_id, float value) override;
	void get_parameters(const char **buffer) override;
	uint8_t get_parameter_count() override { return 1; }

	void reset() override {
		m_phase = 0.f;
		m_noise_timer = 0;
		m_frequency = NAN;
		m_lfsr = 1;
		m_noise_period = 0;
	}

	void render_graphics() override;

	const char *get_id() override { return "org.yoursoftware.sound.2a03"; }
private:
	enum Waveform : uint8_t { PULSE = 0, TRIANGLE = 1, NOISE = 2 };

	enum DutyCycle : uint8_t { D12_5 = 0, D25 = 1, D50 = 2, D75 = 3 };

	uint32_t m_sample_rate = 0;

	float m_phase = 0.f;
	float m_noise_timer = 0.f;
	float m_frequency = NAN;

	uint16_t m_lfsr = 1;
	uint8_t m_noise_period = 0;
	bool m_noise_short_period = false;

	Waveform m_waveform = PULSE;
	DutyCycle m_duty_cycle = D50;
};
