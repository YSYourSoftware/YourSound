#pragma once

#include <SDL3/SDL.h>
#include <YSKMH/MIDIMapping.hpp>

inline YSKMH::MIDIMapping *g_midi_mapping = nullptr;
inline SDL_AudioStream *g_input_stream = nullptr;
inline SDL_AudioStream *g_output_stream = nullptr;
inline uint8_t g_channels;
inline uint32_t g_sample_rate;

bool audio_setup_window(bool refresh);

void audio_meter_window();