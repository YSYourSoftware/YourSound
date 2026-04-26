#pragma once

#include "YourSound/Player.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

namespace YourSound::BinPlayer {
	inline void default_error_func(const char *message) {std::cerr << "[YourSound Default Error Func] " << message << std::endl;}

	inline std::unordered_map<std::string, std::function<Player*()>> g_bp_registry;
	inline std::function<void(const char*)> g_error_func = default_error_func;

	inline YS_CORE_EXPORT_NO_EXTERN void set_error_func(const std::function<void(const char*)> &func) {g_error_func = func;}
	inline YS_CORE_EXPORT_NO_EXTERN void register_player(const std::string &player_id, const std::function<Player*()> &create_func) {g_bp_registry[player_id] = create_func;}
}