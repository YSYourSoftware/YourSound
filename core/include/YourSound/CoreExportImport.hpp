#pragma once

#if defined(_WIN32)

#if defined(YS_IN_CORE_BUILD)
#define YS_API __declspec(dllexport)
#else
#define YS_API __declspec(dllimport)
#endif

#elif defined(__GNUC__) || defined(__clang__)

#if defined(YS_IN_CORE_BUILD)
#define YS_API __attribute__((visibility("default")))
#else
#define YS_API
#endif

#else

#warning "This compiler may not support DLL/SO/DYLIB imports & exports."
#define YS_API

#endif

#define YS_BPI extern "C" __cdecl YS_API
#define YS_BLI YS_BPI
