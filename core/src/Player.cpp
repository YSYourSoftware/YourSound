#include "YourSound/Player.hpp"

#include "YourSound/Rack.hpp"
#include "YourSound/Version.hpp"

#include <SArc/Helpers.hpp>

#include <exception>
#include <iostream>

#include "YourSound/BinPlayerRegistry.hpp"

using namespace YourSound;

PlayerInterface::PlayerInterface(YS_PlayerHandle player_handle, const std::filesystem::path &dll_path) :
	m_player_handle(player_handle), m_dll_path(dll_path) {
	YS_LOAD_API_FN(m_player_api, note_on);
	YS_LOAD_API_FN(m_player_api, note_off);

	YS_LOAD_API_FN(m_player_api, render);

	YS_LOAD_API_FN(m_player_api, store_calc_size);
	YS_LOAD_API_FN(m_player_api, store);
	YS_LOAD_API_FN(m_player_api, load);

	YS_LOAD_API_FN(m_player_api, set_bpm);
	YS_LOAD_API_FN(m_player_api, set_sample_rate);

	YS_LOAD_API_FN(m_player_api, set_parameter);
	YS_LOAD_API_FN(m_player_api, get_parameters);
	YS_LOAD_API_FN(m_player_api, get_parameter_count);

	YS_LOAD_API_FN(m_player_api, render_graphics);

	YS_LOAD_API_FN(m_player_api, reset);

	YS_LOAD_API_FN(m_player_api, get_id);
}

void PlayerInterface::note_on(const uint8_t midi_note_number, const float velocity) {m_player_api.note_on(m_player_handle, midi_note_number, velocity);}
void PlayerInterface::note_off(const uint8_t midi_note_number) {m_player_api.note_off(m_player_handle, midi_note_number);}

void PlayerInterface::render(float *output_buffer, const uint16_t number_samples) {m_player_api.render(m_player_handle, output_buffer, number_samples);}

uint64_t PlayerInterface::store_calc_size(const bool store_reference) const {return m_player_api.store_calc_size(m_player_handle, store_reference);}
void PlayerInterface::store(uint8_t *output_buffer, const bool store_reference) const {m_player_api.store(m_player_handle, output_buffer, store_reference);}
void PlayerInterface::load(const uint8_t *input_buffer) {m_player_api.load(m_player_handle, input_buffer);}

void PlayerInterface::set_bpm(const uint16_t value) {m_player_api.set_bpm(m_player_handle, value);}
void PlayerInterface::set_sample_rate(const uint32_t value) {m_player_api.set_sample_rate(m_player_handle, value);}

void PlayerInterface::set_parameter(const char *param_id, const float value) {m_player_api.set_parameter(m_player_handle, param_id, value);}
void PlayerInterface::get_parameters(const char **buffer) const {m_player_api.get_parameters(m_player_handle, buffer);}
uint8_t PlayerInterface::get_parameter_count() const {return m_player_api.get_parameter_count(m_player_handle);}

void PlayerInterface::render_graphics(YS_ImContextHandle im_context) {m_player_api.render_graphics(m_player_handle, im_context);}

void PlayerInterface::reset() {m_player_api.reset(m_player_handle);}

const char *PlayerInterface::get_id() const {return m_player_api.get_id(m_player_handle);}

#ifdef _WIN32
using get_api_version_fn = uint32_t(*)();
using create_bin_player_fn = YS_RackHandle(*)(const char*);
using destroy_bin_player_fn = void(*)(YS_RackHandle);

void YourSound::free_binary_player(const PlayerInterface *binary_player) {
	const HMODULE dll = loaded_dlls[binary_player->m_dll_path];

	const auto destroy_bin_player = reinterpret_cast<destroy_bin_player_fn>(GetProcAddress(dll, "destroy_bin_player"));
	SARC_RUNTIME_ASSERT(destroy_bin_player, invalid_binary_player, "destroy_bin_player not found in library: " + binary_player->m_dll_path.string());

	destroy_bin_player(binary_player->m_player_handle);
	delete binary_player;
}

PlayerInterface *YourSound::load_binary_player(const std::filesystem::path &dll_path, const std::string &player_id) {
	HMODULE dll;
	if (loaded_dlls.contains(dll_path)) {
		dll = loaded_dlls[dll_path];
	} else {
		dll = LoadLibraryW(reinterpret_cast<LPCWSTR>(dll_path.u16string().c_str()));
		SARC_RUNTIME_ASSERT(dll, SArc::io_error, "Failed to load library: " + dll_path.string());

		loaded_dlls.insert({dll_path, dll});

		const auto get_api_version = reinterpret_cast<get_api_version_fn>(GetProcAddress(dll, "get_api_version"));
		SARC_RUNTIME_ASSERT(get_api_version, invalid_binary_player, "get_api_version not found in library: " + dll_path.string());
		SARC_RUNTIME_ASSERT(get_api_version() == YS_API_VERSION, SArc::version_mismatch, "Player API version mismatch in: " + dll_path.string() + " (expected " + std::to_string(YS_API_VERSION) + ", got " + std::to_string(get_api_version()) + ")");
	}

	const auto create_bin_player = reinterpret_cast<create_bin_player_fn>(GetProcAddress(dll, "create_bin_player"));
	SARC_RUNTIME_ASSERT(create_bin_player, invalid_binary_player, "create_bin_player not found in library: " + dll_path.string());

	YS_RackHandle bin_player = create_bin_player(player_id.c_str());
	SARC_RUNTIME_ASSERT(bin_player, player_not_found, "Player " + player_id + " not found in library: " + dll_path.string());

	return new PlayerInterface(bin_player, dll_path);
}
#else
Player *YourSound::load_binary_player(const std::filesystem::path &dll_path, const std::string &player_id) {
	throw std::runtime_error("Not implemented on this OS");
}

void YourSound::free_binary_player(const PlayerInterface &binary_player) {
	throw std::runtime_error("Not implemented on this OS");
}
#endif

Player *YourSound::load_player_by_id(const std::string &player_id) {
	SARC_RUNTIME_ASSERT(BinPlayer::g_bp_registry.contains(player_id), player_not_found, "Player \"" + player_id + "\" not found");
	return BinPlayer::g_bp_registry[player_id]();
}

PlayerWrapper *YourSound::load_player_wrapper_by_id(const std::string &player_id) {
	SARC_RUNTIME_ASSERT(BinPlayer::g_bp_registry.contains(player_id), player_not_found, "Player \"" + player_id + "\" not found");
	return dynamic_cast<PlayerWrapper*>(BinPlayer::g_bp_registry[player_id]());
}