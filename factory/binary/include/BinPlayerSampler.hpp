#pragma once

#include <YourSound/BinPlayerSTD.hpp>

class BinPlayerSampler : public YourSound::Player {
public:
	void note_on(const uint8_t midi_note_number, const float velocity) override;

	void note_off(const uint8_t midi_note_number) override { m_polyphony_controller.note_off(midi_note_number); }

	void render(float *output_buffer, uint16_t number_samples) override;

	uint64_t store_calc_size(bool store_reference) override;
	void store(uint8_t *output_buffer, bool store_reference) override;
	void load(const uint8_t *input_buffer) override;

	void set_bpm(const uint16_t value) override {}
	void set_sample_rate(const uint32_t value) override { m_polyphony_controller.sample_rate = value; }

	void set_parameter(const char *param_id, float value) override;
	float get_parameter(const char *param_id) override;
	void get_parameters(const char **buffer) override;
	uint8_t get_parameter_count() override;

	void render_graphics() override;

	void reset() override { m_polyphony_controller.reset(); }

	const char *get_id() override { return "org.yoursoftware.sound.sampler"; }
private:
	typedef struct {
		std::filesystem::path loaded_from;
		std::vector<float> pcm;

		float velocity_start;
		float velocity_end;
		float velocity_home;

		uint8_t note_start;
		uint8_t note_end;
		uint8_t note_home;
	} sample_t;

	sample_t *p_get_sample_by_note_info(uint8_t note, float velocity);

	YourSound::BinPlayer::PolyphonyController<256> m_polyphony_controller;

	std::vector<sample_t> m_samples;
	YourSound::BinPlayer::amp_envelope_t m_amp_envelope;
};