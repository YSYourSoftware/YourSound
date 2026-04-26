#define YS_PLUGIN_VERSION_MAJOR 0
#define YS_PLUGIN_VERSION_MINOR 0
#define YS_PLUGIN_VERSION_PATCH 1

#define YS_PLUGIN_ABI_VERSION 0

#if defined(_WIN32)
#define YS_PLUGIN_OS "windows"
#elif defined(__APPLE__)
#define YS_PLUGIN_OS "macos"
#elif defined(__linux__)
#define YS_PLUGIN_OS "linux"
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define YS_PLUGIN_ARCH "x86-64"
#elif defined(__aarch64__) || defined(_M_ARM64)
#define YS_PLUGIN_ARCH "arm64"
#elif defined(__i386__) || defined(_M_IX86)
#define YS_PLUGIN_ARCH "x86"
#elif defined(__arm__) || defined(_M_ARM)
#define YS_PLUGIN_ARCH "arm"
#endif