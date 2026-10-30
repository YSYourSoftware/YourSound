#pragma once

#include "YourSound/Player.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

namespace YourSound::BinPlayer {
	inline void default_error_func(const char *message) {
		std::cerr << "[YourSound Default Error Func] " << message << std::endl;
	}

	inline std::unordered_map<std::string, std::function<Player *()>> g_bp_registry;
	inline std::function g_error_func = default_error_func;

	inline bool default_yesno_func(const char *main_prompt, const char *message, const bool warning) {
		g_error_func("A yes/no question was asked, but no yes/no handler was provided.");
		return false;
	}
	
	inline std::function g_yesno_func = default_yesno_func;

	inline YS_API void set_error_func(const std::function<void(const char *)> &func) { g_error_func = func; }
	inline YS_API void set_yesno_func(const std::function<bool(const char *, const char *, bool)> &func) {
		g_yesno_func = func;
	}

	inline YS_API void register_player(const std::string &player_id, const std::function<Player *()> &create_func) {
		g_bp_registry[player_id] = create_func;
	}
} // namespace YourSound::BinPlayer
