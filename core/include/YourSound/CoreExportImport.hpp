#pragma once

#ifdef YS_IN_CORE_BUILD
#ifdef _WIN32
#define YS_BPI extern "C" __declspec(dllexport)
#define YS_API __declspec(dllexport)
#elifdef __GNUC__
#define YS_BPI extern "C" __attribute__((visibility("default")))
#define YS_API __attribute__((visibility("default")))
#else
#define YS_BPI extern "C"
#define YS_API
#endif
#else
#ifdef _WIN32
#define YS_BPI extern "C" __declspec(dllimport)
#define YS_API __declspec(dllimport)
#else
#define YS_BPI extern "C"
#define YS_API
#endif
#endif