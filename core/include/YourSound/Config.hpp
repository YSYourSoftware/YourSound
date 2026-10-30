#pragma once

#include "YourSound/BinPlayerSTD.hpp"

#include <filesystem>

namespace YourSound {
	YS_API std::filesystem::path get_library_location();
	YS_API std::vector<std::string> get_installed_libraries();
	YS_API std::filesystem::path get_library_file_path(const std::string &library_id, const std::filesystem::path &path);

	YS_API std::filesystem::path get_current_exe_directory();

	YS_API bool is_library_valid(const std::string &id);
}