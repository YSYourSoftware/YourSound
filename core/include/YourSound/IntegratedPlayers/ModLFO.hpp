#pragma once

#include "YourSound/BinPlayerRegistry.hpp"
#include "YourSound/BinPlayerSTD.hpp"
#include "YourSound/Player.hpp"

namespace YourSound::BinPlayer::Integrated {
	struct LFOInfo {
		bool bpm_synced = true;
		bool reversed = false;

		std::string param_id;

		float rate = 1.f;
		float anchor = 0.f;
		float scale = 1.f;

		BasicOscillator osc;
	};

	class YS_CORE_EXPORT_NO_EXTERN ModLFO : public PlayerWrapper {
		public:
			~ModLFO() override;

			void note_on(uint8_t midi_note_number, float velocity) override;
			void note_off(uint8_t midi_note_number) override;

			void render(float *output_buffer, uint16_t number_samples) override;

			uint64_t store_calc_size(bool store_reference) const override;
			void store(uint8_t *output_buffer, bool store_reference) const override;
			void load(const uint8_t *input_buffer) override;

			void set_bpm(const uint16_t value) override {m_bpm = value; m_player->set_bpm(value);}
			void set_sample_rate(const uint32_t value) override {m_sample_rate = value; m_player->set_sample_rate(value);}

			void set_parameter(const char *param_id, const float value) override {m_player->set_parameter(param_id, value);}
			void get_parameters(const char **buffer) const override {return m_player->get_parameters(buffer);}
			uint8_t get_parameter_count() const override {return m_player->get_parameter_count();}

			void render_graphics(YS_ImContextHandle im_context) override;

			void reset() override;

			const char *get_id() const override {return "org.yoursoftware.sound.mod.lfo";}

			void set_wrapped_player(YS_PlayerHandle player) override {m_player = static_cast<Player*>(player);}
		private:
			void p_apply_lfos() const;

			Player *m_player = nullptr;

			uint16_t m_bpm = 0;
			uint32_t m_sample_rate = 0;

			std::vector<LFOInfo> m_lfos;

			float m_time_position = 0.f;
			uint8_t m_render_chunk_size = 1;
	};

	inline YS_CORE_EXPORT_NO_EXTERN void register_mod_lfo() {
		g_bp_registry["org.yoursoftware.sound.mod.lfo"] = [] {
			return new ModLFO();
		};
	}
}
