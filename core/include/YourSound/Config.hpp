#pragma once

#include "BinPlayerSTD.hpp"

#include <filesystem>

namespace YourSound {
	YS_CORE_EXPORT_NO_EXTERN std::filesystem::path get_library_location();
}