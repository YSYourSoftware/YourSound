#include "BinPlayerDrumController.hpp"

#include <YourSound/Serialisation.hpp>

#include <ranges>

constexpr uint16_t EVENT_ACTIVE_MASK = 0x8000;
constexpr uint16_t EVENT_NOTE_MASK = 0x7F00;
constexpr uint16_t EVENT_VELOCITY_MASK = 0x00FF;

constexpr std::array<const char *, 2> SWING_TYPES = {"1/8", "1/16"};
constexpr std::array<const char *, 2> SWING_DIRECTIONS = {"Delay Offbeats", "Advance Offbeats"};

void BinPlayerDrumController::note_on(const uint8_t midi_note_number, const float velocity) {
	if (midi_note_number - 60 >= PATTERN_COUNT) return;

	m_sample_time = 0;
	m_selected_pattern = midi_note_number - 60;
	m_pattern_velocity = velocity;
}

void BinPlayerDrumController::note_off(const uint8_t midi_note_number) {
	if (midi_note_number - 60 != m_selected_pattern) return;
	m_selected_pattern = 255;
}

void BinPlayerDrumController::render(float *output_buffer, const uint16_t number_samples) {
	if (m_selected_pattern == 255) {
		ysbp_player_render(m_player, output_buffer, number_samples);
		return;
	}

	uint16_t samples_consumed = 0;

	if (m_sample_time % m_event_spacing_samples != 0) {
		ysbp_player_render(m_player, output_buffer + (samples_consumed * 2), m_sample_time % m_event_spacing_samples);
		samples_consumed += m_sample_time % m_event_spacing_samples;
		m_sample_time += m_sample_time % m_event_spacing_samples;
		m_sample_time %= m_event_spacing_samples * 4 * m_beats_per_bar * m_pattern_bars[m_selected_pattern];
	}

	if (samples_consumed >= number_samples) return;

	while (samples_consumed < number_samples) {
		const event_cluster_t event_cluster = m_patterns[m_selected_pattern][m_sample_time / m_event_spacing_samples];

		for (uint8_t i = 0; i < 4; i++) {
			const uint16_t event = event_cluster >> (i * 16);
			if ((event & EVENT_ACTIVE_MASK) == 0) continue;

			ysbp_player_note_on(m_player, (event & EVENT_NOTE_MASK) >> 8,
								static_cast<float>(event & EVENT_VELOCITY_MASK) /
									static_cast<float>(EVENT_VELOCITY_MASK) * m_pattern_velocity);
		}

		const uint16_t samples_to_render =
			std::max(m_event_spacing_samples, static_cast<uint16_t>(number_samples - samples_consumed));

		ysbp_player_render(m_player, output_buffer + (samples_consumed * 2), samples_to_render);

		for (uint8_t v = 0; v < 4; v++) {
			const uint16_t event = event_cluster >> (v * 16);
			if ((event & EVENT_ACTIVE_MASK) == 0) continue;

			ysbp_player_note_off(m_player, (event & EVENT_NOTE_MASK) >> 8);
		}

		samples_consumed += samples_to_render;
		m_sample_time += samples_to_render;
		m_sample_time %= m_event_spacing_samples * 4 * m_beats_per_bar * m_pattern_bars[m_selected_pattern];
	}
}

uint64_t BinPlayerDrumController::store_calc_size(const bool store_reference) {
	uint64_t total_patterns_size = 0;
	for (uint8_t i = 0; i < PATTERN_COUNT; i++)
		total_patterns_size += 1 /* bar count */ + (m_patterns[i].size() * 8) /* pattern data */;

	uint64_t total_note_names_size = 0;
	for (const auto &name : m_note_names | std::views::values)
		total_note_names_size += 1 /* note */ + name.size() + 1 /* null */;

	return 1 /* beats per bar */ + 4 /* swing amount */ + 4 /* humanise */ + 1 /* swing type */ +
		   1 /* swing direction */ + total_patterns_size + 1 /* note names size */ + total_note_names_size +
		   std::strlen(ysbp_player_get_id(m_player)) + 1 /* null */ +
		   ysbp_player_store_calc_size(m_player, store_reference);
}

void BinPlayerDrumController::store(uint8_t *output_buffer, const bool store_reference) {
	*output_buffer++ = m_beats_per_bar;

	YourSound::write_float_be(output_buffer, m_swing_amount);
	output_buffer += 4;
	YourSound::write_float_be(output_buffer, m_humanise);
	output_buffer += 4;

	*output_buffer++ = m_swing_type;
	*output_buffer++ = m_swing_direction;

	for (uint8_t i = 0; i < PATTERN_COUNT; i++) {
		*output_buffer++ = m_pattern_bars[i];
		for (const auto &pattern_cluster : m_patterns[i]) {
			YourSound::write_integral_be(output_buffer, pattern_cluster);
			output_buffer += 8;
		}
	}

	*output_buffer++ = m_note_names.size();
	for (const auto &[note, name] : m_note_names) {
		*output_buffer++ = note;
		std::memcpy(output_buffer, name.data(), name.size() + 1);
		output_buffer += name.size() + 1;
	}

	const char *player_id = ysbp_player_get_id(m_player);
	std::memcpy(output_buffer, player_id, std::strlen(player_id) + 1);
	output_buffer += std::strlen(player_id) + 1;

	ysbp_player_store(m_player, output_buffer, store_reference);
}

void BinPlayerDrumController::load(const uint8_t *input_buffer) {
	m_beats_per_bar = *input_buffer++;

	m_swing_amount = YourSound::read_float_be<float>(input_buffer);
	input_buffer += 4;
	m_humanise = YourSound::read_float_be<float>(input_buffer);
	input_buffer += 4;

	m_swing_type = static_cast<SwingType>(*input_buffer++);
	m_swing_direction = static_cast<SwingDirection>(*input_buffer++);

	for (uint8_t i = 0; i < PATTERN_COUNT; i++) {
		m_pattern_bars[i] = *input_buffer++;
		m_patterns[i].reserve(m_pattern_bars[i] * m_beats_per_bar * PATTERN_BEAT_DIVISION);
		for (uint64_t v = 0; v < m_pattern_bars[i] * m_beats_per_bar * PATTERN_BEAT_DIVISION; v++) {
			m_patterns[i][v] = YourSound::read_integral_be<uint64_t>(input_buffer);
			input_buffer += 8;
		}
	}

	const uint8_t note_name_count = *input_buffer++;
	for (uint8_t i = 0; i < note_name_count; i++) {
		const uint8_t note = *input_buffer++;
		m_note_names[note] = std::string(reinterpret_cast<const char *>(input_buffer));
		input_buffer += m_note_names[note].size() + 1;
	}

	m_player = ysbp_create_bin_player(reinterpret_cast<const char *>(input_buffer));
	input_buffer += std::strlen(reinterpret_cast<const char *>(input_buffer)) + 1;
	ysbp_player_load(m_player, input_buffer);
}
