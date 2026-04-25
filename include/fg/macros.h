#pragma once

#if defined(_WIN32) || defined(_WIN64)
#if defined(FG_USE_FG_EXPORT)
#if defined(FG_EXPORT_BUILD)
#define FG_API __declspec(dllexport)
#else
#define FG_API __declspec(dllimport)
#endif
#else
#define FG_API
#endif
#elif defined(__APPLE__) && defined(__MACH__)
#if defined(__GNUC__)
#define FG_API __attribute__((visibility("default")))
#else
#error "Unsupported compiler on macOS. Use Clang or GCC."
#endif
#else
#error "Unsupported platform. Only Windows and macOS are supported."
#endif
