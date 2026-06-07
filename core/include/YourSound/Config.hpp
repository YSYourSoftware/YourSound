#pragma once

#include "BinPlayerSTD.hpp"

#include <filesystem>

namespace YourSound {
	YS_CORE_EXPORT_NO_EXTERN std::filesystem::path get_library_location();
	YS_CORE_EXPORT_NO_EXTERN std::vector<std::string> get_installed_libraries();
	YS_CORE_EXPORT_NO_EXTERN std::filesystem::path get_library_file_path(const std::string &library_id, const std::filesystem::path &path);

	YS_CORE_EXPORT_NO_EXTERN bool is_library_valid(const std::string &id);
}