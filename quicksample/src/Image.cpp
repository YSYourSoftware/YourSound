#include "Image.hpp"

#include <YourSound/BinPlayerSTD.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>

void load_texture_from_memory(const void *data, const size_t size, SDL_Renderer *renderer, SDL_Texture **texture,
							  uint16_t *width, uint16_t *height) {
	int image_width = 0;
	int image_height = 0;
	constexpr uint8_t channels = 4;

	uint8_t *image_data = stbi_load_from_memory(static_cast<const uint8_t *>(data), static_cast<int>(size),
												&image_width, &image_height, nullptr, 4);
	YS_RUNTIME_ASSERT(image_data, std::runtime_error, std::format("Failed to load image: {}", stbi_failure_reason()));

	SDL_Surface *surface =
		SDL_CreateSurfaceFrom(image_width, image_height, SDL_PIXELFORMAT_RGBA32, image_data, channels * image_width);
	YS_RUNTIME_ASSERT(surface, std::runtime_error, std::format("Failed to create surface: {}", SDL_GetError()));

	SDL_Texture *i_texture = SDL_CreateTextureFromSurface(renderer, surface);
	YS_RUNTIME_ASSERT(i_texture, std::runtime_error, std::format("Failed to create texture: {}", SDL_GetError()));

	*texture = i_texture;
	if (height) *width = image_width;
	if (width) *height = image_height;

	SDL_DestroySurface(surface);
	stbi_image_free(image_data);
}

void load_texture_from_file(const std::filesystem::path &path, SDL_Renderer *renderer, SDL_Texture **texture,
							uint16_t *width, uint16_t *height) {
	std::ifstream file{path, std::ios::binary | std::ios::ate};
	const size_t size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> data;
	data.resize(size);

	file.read(data.data(), size);
	file.close();

	load_texture_from_memory(data.data(), size, renderer, texture, width, height);
}
