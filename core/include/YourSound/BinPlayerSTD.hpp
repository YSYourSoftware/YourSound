#pragma once

#include "YourSound/Player.hpp"
#include "YourSound/UI/Controls.hpp"
#include "YourSound/Version.hpp"

#include <cstdint>
#include <iostream>
#include <numbers>
#include <source_location>

#ifdef _WIN32
#define YS_EXTERN_EXPORT extern "C" __declspec(dllexport)
#elifdef __GNUC__
#define YS_EXTERN_EXPORT extern "C" __attribute__((visibility("default")))
#else
#define YS_EXTERN_EXPORT extern "C"
#endif

#ifdef _MSC_VER
#define YS_BP_REG_BUILD_INFO                                                                                           \
	YS_EXTERN_EXPORT const char *get_build_info() {                                                                    \
		return std::format("MSVC {}.{:02}", _MSC_VER / 100, _MSC_VER % 100).c_str();                                   \
	}
#else
#define YS_BP_REG_BUILD_INFO                                                                                           \
	YS_EXTERN_EXPORT                                                                                                   \
	const char *get_build_info() { return "Unknown"; }
#endif

#define YS_START_BP_REGISTRY                                                                                           \
	YS_EXTERN_EXPORT void player_note_on(YS_PlayerHandle player, const uint8_t midi_note_number, float velocity) {     \
		static_cast<YourSound::Player *>(player)->note_on(midi_note_number, velocity);                                 \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT void player_note_off(YS_PlayerHandle player, const uint8_t midi_note_number) {                    \
		static_cast<YourSound::Player *>(player)->note_off(midi_note_number);                                          \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT void player_render(YS_PlayerHandle player, float *output_buffer, const uint16_t number_samples) { \
		static_cast<YourSound::Player *>(player)->render(output_buffer, number_samples);                               \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT uint64_t player_store_calc_size(YS_PlayerHandle player, const bool store_reference) {             \
		return static_cast<YourSound::Player *>(player)->store_calc_size(store_reference);                             \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT void player_store(YS_PlayerHandle player, uint8_t *output_buffer, const bool store_reference) {   \
		static_cast<YourSound::Player *>(player)->store(output_buffer, store_reference);                               \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT void player_load(YS_PlayerHandle player, const uint8_t *input_buffer) {                           \
		static_cast<YourSound::Player *>(player)->load(input_buffer);                                                  \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT void player_set_bpm(YS_PlayerHandle player, const uint16_t value) {                               \
		static_cast<YourSound::Player *>(player)->set_bpm(value);                                                      \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT void player_set_sample_rate(YS_PlayerHandle player, const uint32_t value) {                       \
		static_cast<YourSound::Player *>(player)->set_sample_rate(value);                                              \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT void player_set_parameter(YS_PlayerHandle player, const char *param_id, float value) {            \
		static_cast<YourSound::Player *>(player)->set_parameter(param_id, value);                                      \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT float player_get_parameter(YS_PlayerHandle player, const char *param_id) {                        \
		return static_cast<YourSound::Player *>(player)->get_parameter(param_id);                                      \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT void player_get_parameters(YS_PlayerHandle player, const char **buffer) {                         \
		static_cast<YourSound::Player *>(player)->get_parameters(buffer);                                              \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT uint8_t player_get_parameter_count(YS_PlayerHandle player) {                                      \
		return static_cast<YourSound::Player *>(player)->get_parameter_count();                                        \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT void player_render_graphics(YS_PlayerHandle player) {                                             \
		static_cast<YourSound::Player *>(player)->render_graphics();                                                   \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT void player_reset(YS_PlayerHandle player) { static_cast<YourSound::Player *>(player)->reset(); }  \
                                                                                                                       \
	YS_EXTERN_EXPORT const char *player_get_id(YS_PlayerHandle player) {                                               \
		return static_cast<YourSound::Player *>(player)->get_id();                                                     \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT void destroy_bin_player(YS_PlayerHandle player) {                                                 \
		delete static_cast<YourSound::Player *>(player);                                                               \
	}                                                                                                                  \
                                                                                                                       \
	YS_EXTERN_EXPORT uint32_t get_api_version() { return YS_API_VERSION; }                                             \
	YS_BP_REG_BUILD_INFO                                                                                               \
	YS_EXTERN_EXPORT inline const char *get_yoursound_version() { return YS_VERSION_STRING; }                          \
	YS_EXTERN_EXPORT YS_PlayerHandle create_bin_player(const char *id) {                                               \
		if (!id) return nullptr;
#define YS_REGISTER_BP(bp_id, bp_class)                                                                                \
	if (std::strcmp(id, bp_id) == 0) { return new bp_class(); }
#define YS_END_BP_REGISTRY                                                                                             \
	return nullptr;                                                                                                    \
	}

#define YS_RUNTIME_ASSERT(condition, error_type, message)                                                              \
	do {                                                                                                               \
		if (!(condition)) throw error_type(message);                                                                   \
	} while (0)

namespace YourSound {
	struct ErrorSource {
		const char *function_name;
		const char *file_name;
		uint32_t line_number;
	};

	inline ErrorSource get_error_source(const std::source_location &location = std::source_location::current()) {
		ErrorSource result{};

		result.function_name = location.function_name();
		result.file_name = location.file_name();
		result.line_number = location.line();

		return result;
	}

	namespace BinPlayer {
		enum BasicOscillator : uint8_t { SQUARE = 0, TRIANGLE = 1, SINE = 2, SAWTOOTH = 3, NOISE = 4 };

		typedef struct s_AmpEnvelope {
			float attack_time = 0.f;
			float decay_time = 0.f;
			float sustain_volume = 1.f;
			float release_time = 0.f;
		} amp_envelope_t;

		typedef struct s_FilterEnvelope {
			float attack_time = 0.f;
			float decay_time = 0.f;
			float sustain_level = 1.f;
			float release_time = 0.f;
			float amount = 0.f; // -1.f - +1.f
			float base_cutoff = 1.f;
		} filter_envelope_t;

		[[nodiscard]] inline float midi_to_freq(const uint8_t midi_note, const float pitch_bend = 0.f,
												const float tuning = 440.f) {
			const float semis = (static_cast<float>(midi_note) - 69.f) + pitch_bend * 2.f;
			return tuning * std::exp2f(semis / 12.f);
		}

		[[nodiscard]] inline float calculate_amp_envelope(const amp_envelope_t &envelope, float time,
														  const bool released) {
			time = std::fmaxf(time, 0.f);

			if (released) {
				if (envelope.release_time == 0.f) return 0.f;

				return std::lerp(envelope.sustain_volume, 0.0f,
								 std::fminf(time, envelope.release_time) / envelope.release_time);
			}

			if (time <= envelope.attack_time && envelope.attack_time != 0.f)
				return std::lerp(0.f, 1.f, time / envelope.attack_time);
			if (time < envelope.attack_time + envelope.decay_time && envelope.decay_time != 0.f)
				return std::lerp(1.f, envelope.sustain_volume, (time - envelope.attack_time) / envelope.decay_time);

			return envelope.sustain_volume;
		}

		[[nodiscard]] inline float calculate_basic_osc(const BasicOscillator osc, float time) {
			time = time - std::floorf(time);

			switch (osc) {
			case SQUARE: return (time < 0.5f) ? 1.0f : 0.0f;
			case TRIANGLE: return (time < 0.5f) ? (time * 2.0f) : (2.0f - 2.0f * time);
			case SINE: return 0.5f * (std::sinf(time * 2.0f * std::numbers::pi_v<float>) + 1.0f);
			case SAWTOOTH: return time;
			case NOISE: return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
			}

			return 0.f;
		}

		inline void amp_envelope_edit_ui(amp_envelope_t &envelope) {
			const float drag_widths = 75.f - (ysbp_ui_get_item_spacing().x / 2.f);

			ysbp_ui_set_next_item_width(drag_widths);
			ysbp_ui_float_drag("##amp_a", &envelope.attack_time, 0.05f, 0.f, 10.f, "%.2fs");
			ysbp_ui_same_line();
			ysbp_ui_set_next_item_width(drag_widths);
			ysbp_ui_float_drag("##amp_d", &envelope.decay_time, 0.05f, 0.f, 10.f, "%.2fs");

			ysbp_ui_line_graph(
				"",
				[](void *data, const int idx) -> float {
					const amp_envelope_t *amp_env = static_cast<amp_envelope_t *>(data);
					float total_time_display = amp_env->attack_time + amp_env->decay_time + amp_env->release_time;
					total_time_display += total_time_display * (1.f / 3.f);

					const float time_display = (total_time_display / 149.f) * idx;

					return calculate_amp_envelope(*amp_env,
												  time_display >= total_time_display - amp_env->release_time
													  ? time_display - (total_time_display - amp_env->release_time)
													  : time_display,
												  time_display >= total_time_display - amp_env->release_time);
				},
				&envelope, 150, 0, "", 0.f, 1.f, {150, 50});

			ysbp_ui_set_next_item_width(drag_widths);
			ysbp_ui_float_drag("##amp_s", &envelope.sustain_volume, 0.01f, 0.f, 1.f, "%.2fx");
			ysbp_ui_same_line();
			ysbp_ui_set_next_item_width(drag_widths);
			ysbp_ui_float_drag("##amp_r", &envelope.release_time, 0.05f, 0.f, 10.f, "%.2fs");
		}

		inline void scale_float_array(float *arr, const uint32_t n, const float factor) {
			if (factor == 1.f) return;
			for (uint32_t i = 0; i < n; ++i) arr[i] *= factor;
		}

		inline void mono_to_stereo(const float *mono_array, float *stereo_array, const uint16_t samples) {
			for (uint16_t i = 0; i < samples; ++i) {
				stereo_array[i * 2] = mono_array[i];
				stereo_array[(i * 2) + 1] = mono_array[i];
			}
		}

		template <uint16_t Voices> class PolyphonyController {
		public:
			typedef struct s_Voice {
				float phase_advance = 0.f;
				float phase = 0.f;
				float velocity = 1.f;
				float time = 0.f;

				uint8_t midi_note_number = 0;

				bool hanging_by_sustain = false;
				bool decaying = false;
			} voice_t;

			typedef std::vector<voice_t>::iterator voice_iterator_t;
			typedef std::vector<voice_t>::const_iterator voice_const_iterator_t;

			PolyphonyController() { m_voices.reserve(Voices); }

			void set_sample_rate(const uint32_t sample_rate) { m_sample_rate = sample_rate; }

			void note_on(const uint8_t midi_note_number, const float velocity) {
				if (m_voices.size() >= Voices) { m_voices.erase(m_voices.begin()); }

				m_voices.emplace_back(midi_to_freq(midi_note_number, m_pitch_bend) / m_sample_rate, 0.f, velocity, 0.f,
									  midi_note_number, false, false);
			}

			void note_off(const uint8_t midi_note_number) {
				auto it = std::find_if(m_voices.begin(), m_voices.end(), [midi_note_number](const voice_t &v) {
					return v.midi_note_number == midi_note_number && !v.decaying && !v.hanging_by_sustain;
				});

				if (it != m_voices.end()) {
					if (m_sustaining) {
						it->hanging_by_sustain = true;
						return;
					}

					it->decaying = true;
					it->time = 0.f;
				}
			}

			void voice_delete(const voice_const_iterator_t &it) { m_voices.erase(it); }

			void sustain_on() { m_sustaining = true; }
			void sustain_off() {
				m_sustaining = false;
				for (auto &voice : m_voices) {
					if (!voice.hanging_by_sustain) continue;
					voice.decaying = true;
					voice.time = 0.f;
				}
			}

			void pitch_bend_update(const float pitch_bend) {
				m_pitch_bend = pitch_bend;
				for (auto &voice : m_voices)
					voice.phase_advance = midi_to_freq(voice.midi_note_number, m_pitch_bend) / m_sample_rate;
			}

			void for_each_voice(std::function<void(const voice_iterator_t &)> function) {
				for (voice_iterator_t it = m_voices.begin(); it <= m_voices.end(); ++it) function(it);
			}

			void reset() {
				m_voices.clear();
				m_pitch_bend = 0.f;
				m_sustaining = false;
			}
		private:
			std::vector<voice_t> m_voices;

			float m_pitch_bend = 0.f;
			uint32_t m_sample_rate = 0;

			bool m_sustaining = false;
		};

		class MonophonyController {
		public:
			typedef struct {
				float note_time;
				float velocity;
				float frequency;
			} state_t;

			void reset() {
				m_play_note = 255;
				m_sustaining = false;
				m_notes_on.clear();
			}

			void note_on(const uint8_t midi_note_number, const float velocity) {
				if (m_play_note == 255) {
					m_glide_note = midi_note_number;
					m_glide_velocity = velocity;
				} else {
					m_glide_note = m_play_note;
					m_glide_velocity = m_velocity;
				}

				m_notes_on.push_back(midi_note_number);
				m_play_note = midi_note_number;
				m_velocity = velocity;
				m_note_time = 0.f;
			}

			void note_off(const uint8_t midi_note_number) {
				for (auto it = m_notes_on.begin(); it != m_notes_on.end(); ++it) {
					if (*it == midi_note_number) {
						it = m_notes_on.erase(it);
						if (midi_note_number != m_play_note) break;

						if (m_sustaining) {
							m_cur_note_held_by_sustain = true;
							break;
						}

						if (it == m_notes_on.begin()) {
							m_play_note = 255;
							m_glide_note = 255;
							break;
						}

						--it;

						m_glide_note = midi_note_number;
						m_glide_velocity = m_velocity;
						m_play_note = *it;
						m_note_time = 0.f;
						break;
					}
				}
			}

			void sustain_on() { m_sustaining = true; }
			void sustain_off() {
				m_sustaining = false;

				if (!m_cur_note_held_by_sustain) return;
				m_cur_note_held_by_sustain = false;
				m_play_note = 255;
			}

			state_t get_state(const float time_advance) {
				if (m_play_note == 255) return {0.f, 0.f, 0.f};
				m_note_time += time_advance;

				const float lerp_factor = std::clamp(m_note_time / glide_seconds, 0.f, 1.f);

				return {m_note_time, std::lerp(m_glide_velocity, m_velocity, lerp_factor),
						std::lerp(midi_to_freq(m_glide_note, pitch_bend, tuning),
								  midi_to_freq(m_play_note, pitch_bend, tuning), lerp_factor)};
			}

			float glide_seconds = 0.f;
			float tuning = 0.f;
			float pitch_bend = 0.f;
		private:
			std::vector<uint8_t> m_notes_on;

			float m_note_time = 0.f;
			float m_velocity = 0.f;
			float m_glide_velocity = 0.f;
			uint8_t m_play_note = 255;
			uint8_t m_glide_note = 255;

			bool m_sustaining = false;
			bool m_cur_note_held_by_sustain = false;
		};
	} // namespace BinPlayer
} // namespace YourSound
