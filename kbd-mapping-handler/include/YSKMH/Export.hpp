#pragma once

#ifdef YSKMH_LIBRARY_BUILD
#ifdef _WIN32
#define YSMKH_API __declspec(dllexport)
#elifdef __GNUC__
#define YSMKH_API __attribute__((visibility("default")))
#else
#define YSMKH_API
#endif
#else
#ifdef _WIN32
#define YSMKH_API __declspec(dllimport)
#elifdef __GNUC__
#define YSMKH_API __attribute__((visibility("default")))
#else
#define YSMKH_API
#endif
#endif
