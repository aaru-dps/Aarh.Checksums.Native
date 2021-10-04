#ifndef AARU_CHECKSUMS_NATIVE_LIBRARY_H
#define AARU_CHECKSUMS_NATIVE_LIBRARY_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#if defined(_WIN32)
#define AARU_CALL __stdcall
#define AARU_EXPORT EXTERNC __declspec(dllexport)
#define AARU_LOCAL
#else
#define AARU_CALL
#if defined(__APPLE__)
#define AARU_EXPORT EXTERNC __attribute__((visibility("default")))
#define AARU_LOCAL __attribute__((visibility("hidden")))
#else
#if __GNUC__ >= 4
#define AARU_EXPORT EXTERNC __attribute__((visibility("default")))
#define AARU_LOCAL __attribute__((visibility("hidden")))
#else
#define AARU_EXPORT EXTERNC
#define AARU_LOCAL
#endif
#endif
#endif

#ifdef _MSC_VER
#define FORCE_INLINE static inline
#else
#define FORCE_INLINE static inline __attribute__((always_inline))
#endif

#include "simd.h"

#endif // AARU_CHECKSUMS_NATIVE_LIBRARY_H
