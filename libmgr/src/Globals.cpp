#include "Globals.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void load_texture_from_memory(const SArc::bytes_t &png_data, SDL_Texture **texture_ptr, int &width, int &height, SDL_Renderer *renderer) {
	int channels;

	unsigned char *data = stbi_load_from_memory(
		reinterpret_cast<const unsigned char*>(png_data.data()),
		static_cast<int>(png_data.size()),
		&width,
		&height,
		&channels,
		0
	);

	if (!data) throw std::runtime_error(std::format("Failed to load texture from memory:\n{}", stbi_failure_reason()));

	SDL_PixelFormat format;
	if (channels == 4) format = SDL_PIXELFORMAT_RGBA32; else if (channels == 3) format = SDL_PIXELFORMAT_RGB24; else {
		stbi_image_free(data);
		throw std::runtime_error(std::format("Unsupported channel count: {}", channels));
	}

	SDL_Surface *surface = SDL_CreateSurfaceFrom(width, height, format, data, channels * width);

	if (!surface) {
		stbi_image_free(data);
		throw std::runtime_error(std::format("Failed to create SDL surface:\n{}", SDL_GetError()));
	}

	*texture_ptr = SDL_CreateTextureFromSurface(renderer, surface);

	if (!(*texture_ptr)) throw std::runtime_error(std::format("Failed to create SDL texture:\n{}", SDL_GetError()));

	SDL_DestroySurface(surface);
	stbi_image_free(data);

	if (*texture_ptr == nullptr) throw std::runtime_error("Failed to create SDL texture");
}