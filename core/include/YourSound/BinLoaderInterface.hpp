#pragma once

#include "YourSound/BinLoaderRegistry.hpp"
#include "YourSound/BinPlayerSTD.hpp"
#include "YourSound/CoreExportImport.hpp"

YS_BLI inline void ysbl_show_error(const YourSound::ErrorSource source, const char *message) {
	const std::string final_message = std::string(message) + "\n\nDebug Information:\n\n" +
									  std::string(source.file_name) + ":" + std::to_string(source.line_number) + "\n" +
									  std::string(source.function_name);
	YourSound::BinLoader::g_error_func(final_message.c_str());
}