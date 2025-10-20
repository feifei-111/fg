#pragma once

#if defined(_WIN32) || defined(_WIN64)
#if defined(FG_EXPORT)
#define FG_API __declspec(dllexport)
#else
#define FG_API __declspec(dllimport)
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
