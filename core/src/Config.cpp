#include "YourSound/Config.hpp"

#include <SArc.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

constexpr auto YS_REGISTRY_KEY = L"Software\\YourSoftware\\YourSound";

std::wstring get_reg_string() {
	HKEY hkey;

	LONG result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, YS_REGISTRY_KEY, 0, KEY_READ, &hkey);
	if (result != ERROR_SUCCESS) throw std::runtime_error("Cannot access HKLM/Software/YourSoftware/YourSound");

	DWORD data_size = 0;
	DWORD data_type = 0;

	result = RegQueryValueExW(hkey, L"LibraryLocation", nullptr, &data_type, nullptr, &data_size);
	if (result != ERROR_SUCCESS || data_type != REG_SZ)
		throw std::runtime_error("Cannot access value of LibraryLocation or it is the wrong type");

	std::vector<wchar_t> buffer(data_size / sizeof(wchar_t));

	result = RegQueryValueExW(hkey, L"LibraryLocation", nullptr, nullptr, reinterpret_cast<LPBYTE>(buffer.data()),
							  &data_size);
	if (result != ERROR_SUCCESS || data_type != REG_SZ)
		throw std::runtime_error("Cannot access value of LibraryLocation");

	RegCloseKey(hkey);
	return {buffer.data()};
}

#endif

namespace YourSound {
	std::filesystem::path get_library_location() {
#ifdef _WIN32
		return {get_reg_string()};
#else
		throw std::runtime_error("No config method found");
#endif
	}

	std::vector<std::string> get_installed_libraries() {
		const std::filesystem::path library_location = get_library_location();

		std::vector<std::string> result;

		for (const auto &entry : std::filesystem::directory_iterator(library_location))
			if (entry.is_directory()) result.push_back(entry.path().filename().string());

		return result;
	}

	std::filesystem::path get_library_file_path(const std::string &library_id, const std::filesystem::path &path) {
		const std::filesystem::path library_location = get_library_location();

		YS_RUNTIME_ASSERT(std::filesystem::is_regular_file(library_location / library_id / path),
						  SArc::file_not_found_error,
						  std::format("File {} not found in library {}", path.generic_string(), library_id));

		return library_location / library_id / path;
	}

	bool is_library_valid(const std::string &id) {
		const std::filesystem::path library_location = get_library_location();

		return std::filesystem::is_directory(library_location / id) &&
			   std::filesystem::is_regular_file(library_location / id / "yoursound.json");
	}
} // namespace YourSound
