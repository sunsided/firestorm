//
// Created by sunside on 11.02.18.
//

#ifdef USE_OPENMP

#include <cstddef>
#include <cmath>
#include "firestorm/engine/dot_product_naive.h"

using namespace std;

namespace firestorm {

    float dot_product_openmp(const float *const __restrict__ a_row, const float *const __restrict__ b_row,
                             const size_t N) noexcept {
        auto total = 0.0f;

        #pragma omp simd aligned(a_row, b_row: 32) reduction(+:total)
        for (size_t i = 0; i < N; ++i) {
            total += a_row[i] * b_row[i];
        }
        return total;
    }

    float vec_norm_openmp(const float *const a_row, const size_t N) noexcept {
        const auto squared_norm = dot_product_openmp(a_row, a_row, N);
        return sqrtf(squared_norm);
    }

    float vec_normalize_openmp(float *const a_row, const size_t N) noexcept {
        const auto norm = vec_norm_naive(a_row, N);
        if (norm < FLOAT_ZERO_EPSILON && norm > -FLOAT_ZERO_EPSILON) {
            return 0.0;
        }

        auto n = 1.0f / norm;

        #pragma omp simd aligned(a_row: 32)
        for (size_t i = 0; i < N; ++i) {
            a_row[i] *= n;
        }

        return norm;
    }
}

#endif //OPENMP
