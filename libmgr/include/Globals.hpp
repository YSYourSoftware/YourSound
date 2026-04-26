#pragma once

#include <imgui.h>
#include <SArc.hpp>
#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>

#include <filesystem>

constexpr ImVec4 COLOUR_RED   {255 / 255.f, 064 / 255.f, 064 / 255.f, 1.f};
constexpr ImVec4 COLOUR_GREEN {033 / 255.f, 222 / 255.f, 064 / 255.f, 1.f};
constexpr ImVec4 COLOUR_ORANGE{255 / 255.f, 166 / 255.f, 026 / 255.f, 1.f};

inline ImFont *font_bold;
inline ImFont *font_light;
inline ImFont *font_heading;

inline std::filesystem::path target_library_path;
inline SArc::SArchive target_library;
inline nlohmann::json target_library_info;

inline SDL_Texture *target_library_cover_64;

inline SDL_Renderer *renderer;

inline bool run_main_loop = true;
inline bool display_lib_info = false;
inline bool display_about = false;
inline bool display_install = false;

void load_texture_from_memory(const SArc::bytes_t &png_data, SDL_Texture **texture_ptr, int &width, int &height, SDL_Renderer *renderer);

constexpr ImVec4 colour_scale(const ImVec4 &v, const float s) {
	return ImVec4(v.x * s, v.y * s, v.z * s, v.w);
}

constexpr ImVec4 colour_alpha(const ImVec4 &v, const float a) {
	return ImVec4(v.x, v.y, v.z, a);
}