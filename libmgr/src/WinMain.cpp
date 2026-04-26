#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <shellapi.h>

#include <string>
#include <vector>

extern int main(int argc, char** argv);

inline int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	int argc = 0;
	wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc);

	std::vector<std::string> utf8args(argc);
	std::vector<char*> argv(argc);

	for (int i = 0; i < argc; i++)
	{
		const int size = WideCharToMultiByte(
			CP_UTF8, 0,
			wargv[i], -1,
			nullptr, 0,
			nullptr, nullptr
		);

		utf8args[i].resize(size);

		WideCharToMultiByte(
			CP_UTF8, 0,
			wargv[i], -1,
			utf8args[i].data(), size,
			nullptr, nullptr
		);

		argv[i] = utf8args[i].data();
	}

	LocalFree(wargv);

	return main(argc, argv.data());
}
#endif