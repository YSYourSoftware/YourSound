#pragma once

#include <YourSound/BinPlayerInterface.hpp>
#include <YourSound/Player.hpp>

#include <array>
#include <map>

constexpr uint8_t PATTERN_BEAT_DIVISION = 4; // 4 events per beat (semiquavers)
constexpr uint8_t PATTERN_COUNT = 16;

class BinPlayerDrumController : public YourSound::PlayerWrapper {
public:
	~BinPlayerDrumController() override {
		ysbp_destroy_bin_player(m_player);
		m_player = nullptr;
	}

	void note_on(uint8_t midi_note_number, float velocity) override;
	void note_off(uint8_t midi_note_number) override;

	void render(float *output_buffer, uint16_t number_samples) override;

	[[nodiscard]] uint64_t store_calc_size(bool store_reference) override;
	void store(uint8_t *output_buffer, bool store_reference) override;
	void load(const uint8_t *input_buffer) override;

	void set_bpm(const uint16_t value) override {
		m_bpm = value;
		p_recalc_ev_spacing_samps();
	}

	void set_sample_rate(const uint32_t value) override {
		m_sample_rate = value;
		p_recalc_ev_spacing_samps();
	}

	void set_parameter(const char *param_id, const float value) override {
		ysbp_player_set_parameter(m_player, param_id, value);
	}

	[[nodiscard]] float get_parameter(const char *param_id) override {
		return ysbp_player_get_parameter(m_player, param_id);
	}

	void get_parameters(const char **buffer) override { ysbp_player_get_parameters(m_player, buffer); }
	[[nodiscard]] uint8_t get_parameter_count() override { return ysbp_player_get_parameter_count(m_player); }

	void render_graphics() override;

	void reset() override;

	void set_wrapped_player(YS_PlayerHandle player) override { m_player = player; }
private:
	/**
	 * Per 2-byte cluster:
	 *
	 * MSBy:
	 * The MSBi signifies that this event exists.
	 * The remaining 7 bits are used for the MIDI note number.
	 *
	 * LSBy:
	 * All 8 bits are used for the note velocity.
	 */
	typedef uint64_t event_cluster_t;
	typedef std::vector<event_cluster_t> pattern_t;

	enum SwingType : uint8_t { QUAVER = 0, SEMIQUAVER = 1, SWTY_MAXIMUM };
	enum SwingDirection : uint8_t { DELAY_OFFBEATS = 0, ADVANCE_OFFBEATS = 1, SWDR_MAXIMUM };

	void p_recalc_ev_spacing_samps() {
		m_event_spacing_samples = static_cast<uint32_t>(
			std::round((60.f / static_cast<float>(m_bpm) / PATTERN_BEAT_DIVISION) * static_cast<float>(m_sample_rate)));
	}

	YS_PlayerHandle m_player = nullptr;

	std::map<uint8_t, std::string> m_note_names;

	std::array<pattern_t, PATTERN_COUNT> m_patterns;

	float m_pattern_velocity = 0.f;

	float m_swing_amount = 0.f;
	float m_humanise = 0.f;

	uint32_t m_sample_time = 0;
	uint32_t m_sample_rate = 0;
	uint16_t m_bpm = 0;
	uint16_t m_event_spacing_samples = 0;

	std::array<uint8_t, PATTERN_COUNT> m_pattern_bars{};
	uint8_t m_beats_per_bar = 4;

	uint8_t m_selected_pattern = 0;

	SwingType m_swing_type = QUAVER;
	SwingDirection m_swing_direction = DELAY_OFFBEATS;

	bool m_player_popup_open = false;
};
