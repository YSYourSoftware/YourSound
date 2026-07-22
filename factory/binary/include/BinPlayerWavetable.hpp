#pragma once

#include <YourSound/BinPlayerSTD.hpp>
#include <YourSound/Player.hpp>

#include <array>

inline constexpr uint16_t WAVE_SAMPLE_COUNT = 2048;
inline constexpr uint16_t WAVE_COUNT = 256;

class BinPlayerWavetable : public YourSound::Player {
public:
	void note_on(uint8_t midi_note_number, float velocity) override;
	void note_off(uint8_t midi_note_number) override;

	void render(float *output_buffer, uint16_t number_samples) override;

	uint64_t store_calc_size(bool store_reference) override;
	void store(uint8_t *output_buffer, bool store_reference) override;
	void load(const uint8_t *input_buffer) override;

	void set_bpm(uint16_t value) override {}
	void set_sample_rate(const uint32_t value) override { m_polyphony_controller.set_sample_rate(value); }

	void set_parameter(const char *param_id, float value) override;
	float get_parameter(const char *param_id) override;
	void get_parameters(const char **buffer) override;
	uint8_t get_parameter_count() override { return 4 + 1 + 1; }

	void reset() override;

	void render_graphics() override;

	const char *get_id() override { return "org.yoursoftware.sound.wavetable"; }
private:
	YourSound::BinPlayer::PolyphonyController<128> m_polyphony_controller;

	float m_osc_rate = 0.f;

	float m_wavetable_view_width = 0.f;
	float m_checkbox_width = 0.f;

	YourSound::BinPlayer::amp_envelope_t m_amp_envelope;

	std::array<uint16_t, WAVE_SAMPLE_COUNT * WAVE_COUNT> m_wavetable{};
	uint16_t m_selected_wave = 0;

	YourSound::BinPlayer::BasicOscillator m_osc = YourSound::BinPlayer::SINE;

	bool m_manual_morph = false;
};
