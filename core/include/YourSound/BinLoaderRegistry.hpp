#pragma once

#include "YourSound/CoreExportImport.hpp"
#include "YourSound/Resource.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

namespace YourSound::BinLoader {
	inline void default_error_func(const char *message) {
		std::cerr << "[YourSound Default Error Func] " << message << std::endl;
	}

	inline std::unordered_map<std::string, std::function<ResourceLoader *()>> g_bl_registry;
	inline std::function g_error_func = default_error_func;

	inline YS_API void set_error_func(const std::function<void(const char *)> &func) { g_error_func = func; }
	inline YS_API void register_loader(const std::string &loader_id,
									   const std::function<ResourceLoader *()> &create_func) {
		g_bl_registry[loader_id] = create_func;
	}
} // namespace YourSound::BinLoader
