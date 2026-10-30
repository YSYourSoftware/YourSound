#pragma once

#include <SDL3/SDL.h>

#include <filesystem>

void load_texture_from_memory(const void *data, size_t size, SDL_Renderer *renderer, SDL_Texture **texture,
						  uint16_t *width, uint16_t *height);
void load_texture_from_file(const std::filesystem::path &path, SDL_Renderer *renderer, SDL_Texture **texture,
							uint16_t *width, uint16_t *height);
