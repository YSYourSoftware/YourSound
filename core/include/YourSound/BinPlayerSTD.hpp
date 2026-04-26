#pragma once

#include "YourSound/Player.hpp"
#include "YourSound/Version.hpp"

#include <imgui.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <source_location>

#ifdef _WIN32
#define YS_EXTERN_EXPORT extern "C" __declspec(dllexport)
#else
#define YS_EXTERN_EXPORT extern "C"
#endif

#define YS_START_BP_REGISTRY \
	YS_EXTERN_EXPORT void player_note_on(YS_PlayerHandle player, const uint8_t midi_note_number, float velocity) {static_cast<YourSound::Player*>(player)->note_on(midi_note_number, velocity);} \
	YS_EXTERN_EXPORT void player_note_off(YS_PlayerHandle player, const uint8_t midi_note_number) {static_cast<YourSound::Player*>(player)->note_off(midi_note_number);} \
	\
	YS_EXTERN_EXPORT void player_render(YS_PlayerHandle player, float *output_buffer, const uint16_t number_samples) {static_cast<YourSound::Player*>(player)->render(output_buffer, number_samples);} \
	\
	YS_EXTERN_EXPORT uint64_t player_store_calc_size(YS_PlayerHandle player, const bool store_reference) {return static_cast<YourSound::Player*>(player)->store_calc_size(store_reference);} \
	YS_EXTERN_EXPORT void player_store(YS_PlayerHandle player, uint8_t *output_buffer, const bool store_reference) {static_cast<YourSound::Player*>(player)->store(output_buffer, store_reference);} \
	YS_EXTERN_EXPORT void player_load(YS_PlayerHandle player, const uint8_t *input_buffer) {static_cast<YourSound::Player*>(player)->load(input_buffer);} \
	\
	YS_EXTERN_EXPORT void player_set_bpm(YS_PlayerHandle player, const uint16_t value) {static_cast<YourSound::Player*>(player)->set_bpm(value);} \
	YS_EXTERN_EXPORT void player_set_sample_rate(YS_PlayerHandle player, const uint32_t value) {static_cast<YourSound::Player*>(player)->set_sample_rate(value);} \
	\
	YS_EXTERN_EXPORT void player_set_parameter(YS_PlayerHandle player, const char *param_id, float value) {static_cast<YourSound::Player*>(player)->set_parameter(param_id, value);} \
	YS_EXTERN_EXPORT void player_get_parameters(YS_PlayerHandle player, const char **buffer) {static_cast<YourSound::Player*>(player)->get_parameters(buffer);} \
	YS_EXTERN_EXPORT uint8_t player_get_parameter_count(YS_PlayerHandle player) {return static_cast<YourSound::Player*>(player)->get_parameter_count();} \
	\
	YS_EXTERN_EXPORT void player_render_graphics(YS_PlayerHandle player, YS_ImContextHandle im_context) {static_cast<YourSound::Player*>(player)->render_graphics(im_context);} \
	\
	YS_EXTERN_EXPORT void player_reset(YS_PlayerHandle player) {static_cast<YourSound::Player*>(player)->reset();} \
	\
	YS_EXTERN_EXPORT const char *player_get_id(YS_PlayerHandle player) {return static_cast<YourSound::Player*>(player)->get_id();} \
	\
	YS_EXTERN_EXPORT void destroy_bin_player(YS_PlayerHandle player) {delete static_cast<YourSound::Player*>(player);} \
	YS_EXTERN_EXPORT uint32_t get_api_version() {return YS_API_VERSION;} \
	YS_EXTERN_EXPORT YS_PlayerHandle create_bin_player(const char *id) {if (!id) return nullptr;
#define YS_REGISTER_BP(bp_id, bp_class) if (std::strcmp(id, bp_id) == 0) {return new bp_class();}
#define YS_END_BP_REGISTRY return nullptr;}

namespace YourSound {
	struct ErrorSource {
		const char *function_name;
		const char *file_name;
		uint32_t line_number;
	};

	inline ErrorSource get_error_source(const std::source_location &location = std::source_location::current()) {
		ErrorSource result;

		result.function_name = location.function_name();
		result.file_name = location.file_name();
		result.line_number = location.line();

		return result;
	}
}

namespace YourSound::BinPlayer {
	enum BasicOscillator {
		SQUARE = 0,
		TRIANGLE = 1,
		SINE = 2,
		SAWTOOTH = 3,
		NOISE = 4
	};

	inline void imgui_basic_oscillator_dropdown(BasicOscillator *osc, const char *label) {
		uint8_t current = *osc;

		const char *osc_names[5] = {
			"Square",
			"Triangle",
			"Sine",
			"Sawtooth",
			"Noise"
		};

		if (ImGui::BeginCombo(label, osc_names[current])) {
			for (uint16_t i = 0; i < 5; i++) {
				const bool is_selected = i == current;

				if (ImGui::Selectable(osc_names[i], is_selected)) current = i;
				if (is_selected) ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		*osc = static_cast<BasicOscillator>(current);
	}

	[[nodiscard]] inline float midi_to_freq(const uint8_t midiNote) {
		return 440.f * std::powf(2.f, (static_cast<float>(midiNote) - 69.f) / 12.f);
	}

	[[nodiscard]] inline float calculate_basic_osc(const BasicOscillator osc, float time) {
		time = time - std::floorf(time);

		switch (osc) {
			case SQUARE:
				return (time < 0.5f) ? 1.0f : 0.0f;
			case TRIANGLE:
				return (time < 0.5f) ? (time * 2.0f) : (2.0f - 2.0f * time);
			case SINE:
				return 0.5f * (std::sinf(time) + 1.0f);
			case SAWTOOTH:
				return time;
			case NOISE:
				return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
		}

		return 0.f;
	}

	inline void scale_float_array(float *arr, const uint32_t n, const float factor) {
		if (factor == 1.f) return;
		for (uint32_t i = 0; i < n; ++i) arr[i] *= factor;
	}
}