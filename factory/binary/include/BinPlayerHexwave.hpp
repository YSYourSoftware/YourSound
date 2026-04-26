#pragma once

#include <YourSound/Rack.hpp>

struct HexWave;

class BinPlayerHexwave : public YourSound::Player {
	public:
		BinPlayerHexwave();
		~BinPlayerHexwave() override;

		void note_on(uint8_t midi_note_number, float velocity) override;
		void note_off(uint8_t midi_note_number) override;

		void render(float *output_buffer, uint16_t number_samples) override;

		uint64_t store_calc_size(bool store_reference) const override;
		void store(uint8_t *output_buffer, bool store_reference) const override;
		void load(const uint8_t *input_buffer) override;

		void set_bpm(uint16_t value) override;
		void set_sample_rate(uint32_t value) override;

		void set_parameter(const char *param_id, float value) override;
		void get_parameters(const char **buffer) const override;
		uint8_t get_parameter_count() const override {return 4;}

		void reset() override;

		void render_graphics(YS_ImContextHandle im_context) override;

		const char *get_id() const override {return "org.yoursoftware.sound.hexwave";}
	private:
		uint8_t m_note_on = 0;
		float m_note_velocity = 0.f;

		uint8_t m_bpm = 0;
		uint32_t m_sample_rate = 0;

		float m_top_row_width = 0.f;
		float m_knob_row_width = 0.f;

		bool m_reflect = true;
		float m_peak_time = 0.f;
		float m_half_height = 1.f;
		float m_zero_wait = 0.f;

		float *m_hexwave_user_buffer = nullptr;

		HexWave *m_hexwave = nullptr;

		void p_update_params() const;
};