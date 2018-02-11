//
// Created by Markus on 12.04.2017.
//

#ifndef FIRESTORM_SIMD_H
#define FIRESTORM_SIMD_H

#include <x86intrin.h>

#ifdef __AVX2__
static inline bool avx2_enabled() noexcept { return __builtin_cpu_supports("avx2"); }
#else
static inline bool avx2_enabled() noexcept { return false; }
#endif

#ifdef __AVX__
static inline bool avx_enabled() noexcept { return __builtin_cpu_supports("avx"); }
#else
static inline bool avx_enabled() noexcept { return false; }
#endif

#endif //FIRESTORM_SIMD_H
