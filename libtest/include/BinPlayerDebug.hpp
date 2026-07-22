#pragma once

#include <YourSound/Player.hpp>

#include <unordered_map>

class BinPlayerDebug : public YourSound::PlayerWrapper {
public:
	~BinPlayerDebug() override;

	void note_on(uint8_t midi_note_number, float velocity) override;
	void note_off(uint8_t midi_note_number) override;

	void render(float *output_buffer, const uint16_t number_samples) override {
		m_player->render(output_buffer, number_samples);
	}

	[[nodiscard]] uint64_t store_calc_size(const bool store_reference) override {
		return m_player->store_calc_size(store_reference);
	}

	void store(uint8_t *output_buffer, const bool store_reference) override {
		m_player->store(output_buffer, store_reference);
	}

	void load(const uint8_t *input_buffer) override { m_player->load(input_buffer); }

	void set_bpm(uint16_t value) override;
	void set_sample_rate(uint32_t value) override;

	void set_parameter(const char *param_id, float value) override;
	[[nodiscard]] float get_parameter(const char *param_id) override { return m_player->get_parameter(param_id); }
	void get_parameters(const char **buffer) override { m_player->get_parameters(buffer); }
	[[nodiscard]] uint8_t get_parameter_count() override { return m_player->get_parameter_count(); }

	void render_graphics() override;

	void reset() override;

	[[nodiscard]] const char *get_id() override { return m_player->get_id(); }

	void set_wrapped_player(YS_PlayerHandle player) override { m_player = static_cast<Player *>(player); }
private:
	Player *m_player = nullptr;

	std::unordered_map<std::string, float> m_parameters;

	uint32_t m_sample_rate = 0;
	uint16_t m_bpm = 0;

	uint16_t m_notes_on = 0;
};
