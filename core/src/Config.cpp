#include "YourSound/Config.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

constexpr LPCWSTR YS_REGISTRY_KEY = L"Software\\YourSoftware\\YourSound";

std::wstring get_reg_string() {
	HKEY hkey;

	LONG result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, YS_REGISTRY_KEY, 0, KEY_READ, &hkey);
	if (result != ERROR_SUCCESS) throw std::runtime_error("Cannot access HKLM/Software/YourSoftware/YourSound");

	DWORD data_size = 0;
	DWORD data_type = 0;

	result = RegQueryValueExW(hkey, L"LibraryLocation", nullptr, &data_type, nullptr, &data_size);
	if (result != ERROR_SUCCESS || data_type != REG_SZ) throw std::runtime_error("Cannot access value of LibraryLocation or it is the wrong type");

	std::vector<wchar_t> buffer(data_size / sizeof(wchar_t));

	result = RegQueryValueExW(hkey, L"LibraryLocation", nullptr, nullptr, reinterpret_cast<LPBYTE>(buffer.data()), &data_size);
	if (result != ERROR_SUCCESS || data_type != REG_SZ) throw std::runtime_error("Cannot access value of LibraryLocation");

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
	};
}