#pragma once

#include "BinPlayerSTD.hpp"
#include "YourSound/BinPlayerRegistry.hpp"
#include "YourSound/Rack.hpp"

YS_CORE_EXPORT void ysbp_rack_add_player(YS_RackHandle rack, YS_PlayerHandle player);
YS_CORE_EXPORT void ysbp_rack_wrap_player(YS_RackHandle rack, YS_PlayerHandle player, YS_PlayerWrapperHandle wrapper);

YS_CORE_EXPORT inline void ysbp_player_note_on(YS_PlayerHandle player, const uint8_t midi_note_number, float velocity) {static_cast<YourSound::Player*>(player)->note_on(midi_note_number, velocity);}
YS_CORE_EXPORT inline void ysbp_player_note_off(YS_PlayerHandle player, const uint8_t midi_note_number) {static_cast<YourSound::Player*>(player)->note_off(midi_note_number);}

YS_CORE_EXPORT inline void ysbp_player_render(YS_PlayerHandle player, float *output_buffer, const uint16_t number_samples) {static_cast<YourSound::Player*>(player)->render(output_buffer, number_samples);}

YS_CORE_EXPORT inline uint64_t ysbp_player_store_calc_size(YS_PlayerHandle player, const bool store_reference) {return static_cast<YourSound::Player*>(player)->store_calc_size(store_reference);}
YS_CORE_EXPORT inline void ysbp_player_store(YS_PlayerHandle player, uint8_t *output_buffer, const bool store_reference) {static_cast<YourSound::Player*>(player)->store(output_buffer, store_reference);}
YS_CORE_EXPORT inline void ysbp_player_load(YS_PlayerHandle player, const uint8_t *input_buffer) {static_cast<YourSound::Player*>(player)->load(input_buffer);}

YS_CORE_EXPORT inline void ysbp_player_set_bpm(YS_PlayerHandle player, const uint16_t value) {static_cast<YourSound::Player*>(player)->set_bpm(value);}
YS_CORE_EXPORT inline void ysbp_player_set_sample_rate(YS_PlayerHandle player, const uint32_t value) {static_cast<YourSound::Player*>(player)->set_sample_rate(value);}

YS_CORE_EXPORT inline void ysbp_player_set_parameter(YS_PlayerHandle player, const char *param_id, float value) {static_cast<YourSound::Player*>(player)->set_parameter(param_id, value);}
YS_CORE_EXPORT inline void ysbp_player_get_parameters(YS_PlayerHandle player, const char **buffer) {static_cast<YourSound::Player*>(player)->get_parameters(buffer);}
YS_CORE_EXPORT inline uint8_t ysbp_player_get_parameter_count(YS_PlayerHandle player) {return static_cast<YourSound::Player*>(player)->get_parameter_count();}

YS_CORE_EXPORT inline void ysbp_player_render_graphics(YS_PlayerHandle player, YS_ImContextHandle im_context) {static_cast<YourSound::Player*>(player)->render_graphics(im_context);}

YS_CORE_EXPORT inline void ysbp_player_reset(YS_PlayerHandle player) {static_cast<YourSound::Player*>(player)->reset();}

YS_CORE_EXPORT inline const char *ysbp_player_get_id(YS_PlayerHandle player) {return static_cast<YourSound::Player*>(player)->get_id();}

YS_CORE_EXPORT inline void ysbp_destroy_bin_player(YS_PlayerHandle player) {delete static_cast<YourSound::Player*>(player);}

YS_CORE_EXPORT inline void ysbp_show_error(const YourSound::ErrorSource source, const char *message) {
	const std::string final_message = std::string(message) + "\n\nDebug Information:\n\n" + std::string(source.file_name) + ":" + std::to_string(source.line_number) + "\n" + std::string(source.function_name) ;
	YourSound::BinPlayer::g_error_func(final_message.c_str());
}