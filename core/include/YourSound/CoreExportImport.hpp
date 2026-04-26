#pragma once

#ifdef YS_IN_CORE_BUILD
#ifdef _WIN32
#define YS_CORE_EXPORT extern "C" __declspec(dllexport)
#define YS_CORE_EXPORT_NO_EXTERN __declspec(dllexport)
#else
#define YS_CORE_EXPORT extern "C"
#define YS_CORE_EXPORT_NO_EXTERN
#endif
#else
#ifdef _WIN32
#define YS_CORE_EXPORT extern "C" __declspec(dllimport)
#define YS_CORE_EXPORT_NO_EXTERN __declspec(dllimport)
#else
#define YS_CORE_EXPORT extern "C"
#define YS_CORE_EXPORT_NO_EXTERN
#endif
#endif