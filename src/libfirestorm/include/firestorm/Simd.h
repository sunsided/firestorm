//
// Created by Markus on 12.04.2017.
//

#ifndef FIRESTORM_SIMD_H
#define FIRESTORM_SIMD_H

#include <x86intrin.h>

static inline bool avx2_available() noexcept { return __builtin_cpu_supports("avx2"); }
#ifdef __AVX2__
static inline bool avx2_enabled() noexcept { avx2_available(); }
#else
static inline bool avx2_enabled() noexcept { return false; }
#endif

static inline bool avx_available() noexcept { return __builtin_cpu_supports("avx"); }
#ifdef __AVX__
static inline bool avx_enabled() noexcept { return avx_available(); }
#else
static inline bool avx_enabled() noexcept { return false; }
#endif

static inline bool sse42_available() noexcept { return __builtin_cpu_supports("sse4.2"); }
#ifdef USE_SSE
static inline bool sse42_enabled() noexcept { return sse42_available(); }
#else
static inline bool sse42_enabled() noexcept { return false; }
#endif

#endif //FIRESTORM_SIMD_H
