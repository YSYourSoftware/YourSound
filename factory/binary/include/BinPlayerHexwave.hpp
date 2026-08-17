#pragma once

#include <YourSound/BinPlayerSTD.hpp>
#include <YourSound/Player.hpp>

struct HexWave;

class BinPlayerHexwave : public YourSound::Player {
public:
	BinPlayerHexwave();
	~BinPlayerHexwave() override;

	void note_on(uint8_t midi_note_number, float velocity) override;
	void note_off(uint8_t midi_note_number) override;

	void render(float *output_buffer, uint16_t number_samples) override;

	uint64_t store_calc_size(bool store_reference) override;
	void store(uint8_t *output_buffer, bool store_reference) override;
	void load(const uint8_t *input_buffer) override;

	void set_bpm(uint16_t value) override {};
	void set_sample_rate(uint32_t value) override;

	void set_parameter(const char *param_id, float value) override;
	float get_parameter(const char *param_id) override;
	void get_parameters(const char **buffer) override;
	uint8_t get_parameter_count() override { return 4; }

	void reset() override;

	void render_graphics() override;

	const char *get_id() override { return "org.yoursoftware.sound.hexwave"; }
private:
	YourSound::BinPlayer::MonophonyController m_monophony_controller;
	YourSound::BinPlayer::amp_envelope_t m_amp_envelope;
	YourSound::BinPlayer::filter_envelope_t m_filter_envelope;

	HexWave *m_hexwave = nullptr;
	float *m_hexwave_user_buffer = nullptr;

	uint32_t m_sample_rate = 0;

	float m_top_row_width = 0.f;
	float m_knob_row_width = 0.f;
	float m_envelope_row_width = 0.f;
	float m_envelope_row_height = 0.f;

	float m_peak_time = 0.f;
	float m_half_height = 1.f;
	float m_zero_wait = 0.f;
	bool m_reflect = true;

	void p_update_params() const;
};
