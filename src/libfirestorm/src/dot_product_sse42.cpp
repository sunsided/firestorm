//
// Created by sunside on 11.02.18.
//

#if USE_SSE == 4

#include <cstddef>
#include <smmintrin.h>
#include "firestorm/dot_product_sse42.h"

using namespace std;

// https://stackoverflow.com/a/35270026/195651

float dot_product_sse42(const float *const __restrict__ a_row, const float *const __restrict__ b_row, const size_t N) noexcept {
    auto total = _mm_setzero_ps();
    for (size_t i = 0; i < N; i += 4) {
        const auto a0 = _mm_load_ps(&a_row[i]);
        const auto b0 = _mm_load_ps(&b_row[i]);

        const auto result = _mm_dp_ps(a0, b0, 0xff);
        total = _mm_add_ps(total, result);
    }

    alignas(16) float ptr[8];
    _mm_store_ps(ptr, total);
    return ptr[0];
}

float vec_norm_sse42(const float *const a_row, const size_t N) noexcept {
    auto total = _mm_setzero_ps();
    for (size_t i = 0; i < N; i += 4) {
        const auto a0 = _mm_load_ps(&a_row[i]);

        const auto result = _mm_dp_ps(a0, a0, 0xff);
        total = _mm_add_ps(total, result);
    }

    total = _mm_sqrt_ps(total);

    alignas(16) float ptr[8];
    _mm_store_ps(ptr, total);
    return ptr[0];
}

float vec_normalize_sse42(float* a_row, std::size_t N) noexcept {
    const auto norm = vec_norm_sse42(a_row, N);
    if (norm == 0.0f) {
        return 0.0;
    }

    auto n = _mm_set1_ps(1.0f/norm);
    for (size_t i = 0; i < N; i += 4) {
        const auto a0 = _mm_load_ps(&a_row[i]);
        const auto c0 = _mm_mul_ps(a0, n);
        _mm_store_ps(&a_row[i], c0);
    }

    return norm;
}

#endif //SSE_VERSION