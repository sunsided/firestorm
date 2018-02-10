//
// Created by sunside on 10.02.18.
//

#ifndef FIRESTORM_DOT_PRODUCT_NAIVE_H
#define FIRESTORM_DOT_PRODUCT_NAIVE_H

#include <memory>

inline float dot_product_naive(const float *const __restrict__ a_row, const float *const __restrict__ b_row, const size_t N) {
    auto total = 0.0f;
    for (size_t i = 0; i < N; ++i) {
        total += a_row[i] * b_row[i];
    }
    return total;
}

inline float dot_product_unrolled_8(const float *const __restrict__ a_row, const float *const __restrict__ b_row, const size_t N) {
    auto total = 0.0f;
    for (size_t i = 0; i < N; i += 8) {
        total += a_row[i] * b_row[i] +
                 a_row[i + 1] * b_row[i + 1] +
                 a_row[i + 2] * b_row[i + 2] +
                 a_row[i + 3] * b_row[i + 3] +
                 a_row[i + 4] * b_row[i + 4] +
                 a_row[i + 5] * b_row[i + 5] +
                 a_row[i + 6] * b_row[i + 6] +
                 a_row[i + 7] * b_row[i + 7];
    }
    return total;
}

float vec_norm_naive(const float *const a_row, const size_t N) {
    const auto squared_norm = dot_product_unrolled_8(a_row, a_row, N);
    return sqrtf(squared_norm);
}

float vec_normalize_naive(float *const a_row, const size_t N) {
    const auto norm = vec_norm_naive(a_row, N);

    auto n = 1.0f / norm;
    for (size_t i = 0; i < N; ++i) {
        a_row[i] *= n;
    }

    return norm;
}

#endif //FIRESTORM_DOT_PRODUCT_NAIVE_H
