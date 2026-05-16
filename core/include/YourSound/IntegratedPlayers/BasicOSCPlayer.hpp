#pragma once

#include "YourSound/BinPlayerRegistry.hpp"
#include "YourSound/BinPlayerSTD.hpp"
#include "YourSound/Player.hpp"

namespace YourSound::BinPlayer::Integrated {
	class YS_CORE_EXPORT_NO_EXTERN BasicOSCPlayer : public Player {
		public:
			void note_on(const uint8_t midi_note_number, const float velocity) override {m_note_midi = midi_note_number; m_note_velocity = velocity;}
			void note_off(const uint8_t midi_note_number) override {m_note_midi = 0;}

			void render(float *output_buffer, uint16_t number_samples) override;

			uint64_t store_calc_size(bool store_reference) const override {return 1;}
			void store(uint8_t *output_buffer, bool store_reference) const override {output_buffer[0] = m_osc;}
			void load(const uint8_t *input_buffer) override {m_osc = static_cast<BasicOscillator>(input_buffer[0]);}

			void set_bpm(const uint16_t value) override {}
			void set_sample_rate(const uint32_t value) override {m_sample_rate = value;}

			void set_parameter(const char *param_id, float value) override;
			void get_parameters(const char **buffer) const override {buffer[0] = "osc";}
			uint8_t get_parameter_count() const override {return 1;}

			void render_graphics(YS_ImContextHandle im_context) override;

			void reset() override {m_note_midi = 0; m_note_velocity = 0.f; m_pitch_bend = 0.f;}

			const char *get_id() const override {return "org.yoursoftware.sound.basic-osc";}
		private:
			BasicOscillator m_osc = SQUARE;
			float m_phase = 0.f;

			uint32_t m_sample_rate = 0;

			uint8_t m_note_midi = 0;
			float m_note_velocity = 0.f;

			float m_pitch_bend = 0.f;
	};

	inline YS_CORE_EXPORT_NO_EXTERN void register_basic_osc_player() {
		g_bp_registry["org.yoursoftware.sound.basic-osc"] = [] {
			return new BasicOSCPlayer();
		};
	}
}