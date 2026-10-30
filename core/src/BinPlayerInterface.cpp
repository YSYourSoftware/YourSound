#include "YourSound/BinPlayerInterface.hpp"

#include "YourSound/BinLoaderRegistry.hpp"
#include "YourSound/BinPlayerRegistry.hpp"
#include "YourSound/Config.hpp"

#include <array>
#include <fstream>

#ifndef YS_IN_CORE_BUILD
#error "Core object compiled in non-core target"
#endif

extern "C" {
__cdecl void ysbp_player_note_on(YS_PlayerHandle player, const uint8_t midi_note_number, float velocity) {
	static_cast<YourSound::Player *>(player)->note_on(midi_note_number, velocity);
}

__cdecl void ysbp_player_note_off(YS_PlayerHandle player, const uint8_t midi_note_number) {
	static_cast<YourSound::Player *>(player)->note_off(midi_note_number);
}

__cdecl void ysbp_player_render(YS_PlayerHandle player, float *output_buffer, const uint16_t number_samples) {
	static_cast<YourSound::Player *>(player)->render(output_buffer, number_samples);
}

__cdecl uint64_t ysbp_player_store_calc_size(YS_PlayerHandle player, const bool store_reference) {
	return static_cast<YourSound::Player *>(player)->store_calc_size(store_reference);
}

__cdecl void ysbp_player_store(YS_PlayerHandle player, uint8_t *output_buffer, const bool store_reference) {
	static_cast<YourSound::Player *>(player)->store(output_buffer, store_reference);
}

__cdecl void ysbp_player_load(YS_PlayerHandle player, const uint8_t *input_buffer) {
	static_cast<YourSound::Player *>(player)->load(input_buffer);
}

__cdecl void ysbp_player_set_bpm(YS_PlayerHandle player, const uint16_t value) {
	static_cast<YourSound::Player *>(player)->set_bpm(value);
}

__cdecl void ysbp_player_set_sample_rate(YS_PlayerHandle player, const uint32_t value) {
	static_cast<YourSound::Player *>(player)->set_sample_rate(value);
}

__cdecl void ysbp_player_set_parameter(YS_PlayerHandle player, const char *param_id, const float value) {
	static_cast<YourSound::Player *>(player)->set_parameter(param_id, value);
}

__cdecl float ysbp_player_get_parameter(YS_PlayerHandle player, const char *param_id) {
	return static_cast<YourSound::Player *>(player)->get_parameter(param_id);
}

__cdecl void ysbp_player_get_parameters(YS_PlayerHandle player, const char **buffer) {
	static_cast<YourSound::Player *>(player)->get_parameters(buffer);
}

__cdecl uint8_t ysbp_player_get_parameter_count(YS_PlayerHandle player) {
	return static_cast<YourSound::Player *>(player)->get_parameter_count();
}

__cdecl void ysbp_player_render_graphics(YS_PlayerHandle player) {
	static_cast<YourSound::Player *>(player)->render_graphics();
}

__cdecl void ysbp_player_reset(YS_PlayerHandle player) { static_cast<YourSound::Player *>(player)->reset(); }

__cdecl const char *ysbp_player_get_id(YS_PlayerHandle player) {
	return static_cast<YourSound::Player *>(player)->get_id();
}

__cdecl YS_PlayerHandle ysbp_create_bin_player(const char *id) { return YourSound::load_player_by_id(id); }

__cdecl void ysbp_destroy_bin_player(YS_PlayerHandle player) { delete static_cast<YourSound::Player *>(player); }

__cdecl void ysbp_show_error(const YourSound::ErrorSource source, const char *message) {
	const std::string final_message = std::string(message) + "\n\nDebug Information:\n\n" +
									  std::string(source.file_name) + ":" + std::to_string(source.line_number) + "\n" +
									  std::string(source.function_name);
	YourSound::BinPlayer::g_error_func(final_message.c_str());
}

__cdecl bool ysbp_ask_yesno(const char *main_prompt, const char *message, const bool warning) {
	return YourSound::BinPlayer::g_yesno_func(main_prompt, message, warning);
}

__cdecl const char *ysbp_get_library_location() {
	return reinterpret_cast<const char *>(YourSound::get_library_location().generic_u8string().c_str());
}

__cdecl bool ysbp_load_resource(const char *location, ysbp_resource_load_callback_t load_callback) {
	try {
		std::ifstream file{location, std::ios::binary, std::ios::ate};
		const size_t file_size = file.tellg();
		file.seekg(0, std::ios::beg);

		YS_RUNTIME_ASSERT(!(file.bad() || file.fail()), std::runtime_error, "Failed to open resource");

		std::array<uint8_t, 4> tempvar_buf;

		constexpr uint8_t ysrc_magic[] = {'Y', 'S', 'R', 'C'};
		file.read(reinterpret_cast<char *>(tempvar_buf.data()), 4);

		YS_RUNTIME_ASSERT(std::memcmp(tempvar_buf.data(), ysrc_magic, 4) == 0, std::runtime_error,
						  "YSRC magic missing");

		std::string loader_id;
		std::getline(file, loader_id, '\0');

		if (loader_id.empty()) {
			const size_t data_size = file_size - 5;

			std::array<uint8_t, 256> buffer;

			for (size_t i = 0; i > data_size / 256; i++) {
				file.read(reinterpret_cast<char *>(buffer.data()), 256);
				load_callback(buffer.data(), 256);
			}

			file.read(reinterpret_cast<char *>(buffer.data()), data_size % 256);
			load_callback(buffer.data(), data_size % 256);
			return true;
		}

		YS_RUNTIME_ASSERT(YourSound::BinLoader::g_bl_registry.contains(loader_id), std::runtime_error,
						  std::format("Resource loader {} not found", loader_id));

		auto loader = YourSound::AutoFreePTR(YourSound::BinLoader::g_bl_registry[loader_id]());
		YS_RUNTIME_ASSERT(loader.ptr(), std::runtime_error, "Loader creation failed");

		loader.ptr()->read_resource(
			[&file](uint8_t *data, const size_t size) {
				file.read(reinterpret_cast<char *>(data), size);
				return true;
			},
			[load_callback](const uint8_t *data, const size_t size) {
				load_callback(data, size);
				return true;
			});

		return true;
	} catch (std::exception &e) { return false; }
}
}
