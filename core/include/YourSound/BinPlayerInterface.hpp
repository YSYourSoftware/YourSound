#pragma once

#include "YourSound/BinPlayerSTD.hpp"

extern "C" {
typedef void(__cdecl *ysbp_resource_load_callback_t)(const uint8_t *, size_t);
typedef void(__cdecl *ysbp_resource_store_callback_t)(uint8_t *, size_t);
}

YS_BPI void ysbp_player_note_on(YS_PlayerHandle player, uint8_t midi_note_number, float velocity);
YS_BPI void ysbp_player_note_off(YS_PlayerHandle player, uint8_t midi_note_number);

YS_BPI void ysbp_player_render(YS_PlayerHandle player, float *output_buffer, uint16_t number_samples);

YS_BPI uint64_t ysbp_player_store_calc_size(YS_PlayerHandle player, bool store_reference);
YS_BPI void ysbp_player_store(YS_PlayerHandle player, uint8_t *output_buffer, bool store_reference);
YS_BPI void ysbp_player_load(YS_PlayerHandle player, const uint8_t *input_buffer);

YS_BPI void ysbp_player_set_bpm(YS_PlayerHandle player, uint16_t value);
YS_BPI void ysbp_player_set_sample_rate(YS_PlayerHandle player, uint32_t value);

YS_BPI void ysbp_player_set_parameter(YS_PlayerHandle player, const char *param_id, float value);
YS_BPI float ysbp_player_get_parameter(YS_PlayerHandle player, const char *param_id);
YS_BPI void ysbp_player_get_parameters(YS_PlayerHandle player, const char **buffer);
YS_BPI uint8_t ysbp_player_get_parameter_count(YS_PlayerHandle player);

YS_BPI void ysbp_player_render_graphics(YS_PlayerHandle player);

YS_BPI void ysbp_player_reset(YS_PlayerHandle player);

YS_BPI const char *ysbp_player_get_id(YS_PlayerHandle player);

YS_BPI YS_PlayerHandle ysbp_create_bin_player(const char *id);
YS_BPI void ysbp_destroy_bin_player(YS_PlayerHandle player);

YS_BPI void ysbp_show_error(YourSound::ErrorSource source, const char *message);

YS_BPI bool ysbp_ask_yesno(const char *main_prompt, const char *message, bool warning);

YS_BPI const char *ysbp_get_library_location();
YS_BPI bool ysbp_load_resource(const char *location, ysbp_resource_load_callback_t load_callback);
/*YS_BPI bool ysbp_store_resource(const char *location, const char *loader,
								ysbp_resource_store_callback_t store_callback);*/
