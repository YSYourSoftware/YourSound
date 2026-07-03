#pragma once

#include <YourSound/BinPlayerSTD.hpp>

class BinPlayerSampler : public YourSound::Player {
	public:
		void note_on(uint8_t midi_note_number, float velocity) override;
		void note_off(uint8_t midi_note_number) override;

		void render(float *output_buffer, uint16_t number_samples) override;

		uint64_t store_calc_size(const bool store_reference) override;
		void store(uint8_t *output_buffer, bool store_reference) override;
		void load(const uint8_t *input_buffer) override;

		void set_bpm(const uint16_t value) override {}
		void set_sample_rate(const uint32_t value) override {m_sample_rate = value;}

		void set_parameter(const char *param_id, float value) override;
		void get_parameters(const char **buffer) override;
		uint8_t get_parameter_count() override;

		void render_graphics() override;

		void reset() override {m_pitch_bend = 0.f;}

		const char *get_id() override {return "org.yoursoftware.sound.sampler";}
	private:
		float m_sample_rate = 0.f;
		float m_pitch_bend = 0.f;


};