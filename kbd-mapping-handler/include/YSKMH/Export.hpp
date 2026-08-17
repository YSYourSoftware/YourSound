#pragma once

#ifdef YSKMH_LIBRARY_BUILD
#ifdef _WIN32
#define YSKMH_API __declspec(dllexport)
#elifdef __GNUC__
#define YSKMH_API __attribute__((visibility("default")))
#else
#define YSKMH_API
#endif
#else
#ifdef _WIN32
#define YSKMH_API __declspec(dllimport)
#elifdef __GNUC__
#define YSKMH_API __attribute__((visibility("default")))
#else
#define YSKMH_API
#endif
#endif
