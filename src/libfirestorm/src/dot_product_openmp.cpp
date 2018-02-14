//
// Created by sunside on 11.02.18.
//

#if OPENMP

#include <cstddef>
#include <cmath>
#include "firestorm/dot_product_naive.h"

using namespace std;

float dot_product_openmp(const float *const __restrict__ a_row, const float *const __restrict__ b_row, const ptrdiff_t N) noexcept {
    auto total = 0.0f;

    #pragma omp simd aligned(a_row, b_row: 32) reduction(+:total)
    for (ptrdiff_t i = 0; i < N; ++i) {
        total += a_row[i] * b_row[i];
    }
    return total;
}

float vec_norm_openmp(const float *const a_row, const ptrdiff_t N) noexcept {
    const auto squared_norm = dot_product_openmp(a_row, a_row, N);
    return sqrtf(squared_norm);
}

float vec_normalize_openmp(float *const a_row, const ptrdiff_t N) noexcept {
    const auto norm = vec_norm_naive(a_row, N);
    auto n = 1.0f / norm;

    #pragma omp simd aligned(a_row: 32)
    for (ptrdiff_t i = 0; i < N; ++i) {
        a_row[i] *= n;
    }

    return norm;
}

#endif //OPENMP