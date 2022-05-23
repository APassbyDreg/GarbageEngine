#pragma once

#if defined(_WIN32)
#ifdef GE_BUILD_RUNTIME
#define GE_API __declspec(dllexport)
#else
#define GE_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
#define GE_API __attribute__((visibility("default")))
#else
#define GE_API
#endif