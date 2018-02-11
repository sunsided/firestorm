//
// Created by Markus on 28.05.2017.
//

#ifndef FIRESTORM_DOT_PRODUCT_AVX256_H
#define FIRESTORM_DOT_PRODUCT_AVX256_H

#if AVX2 || AVX

#include <memory>
#include "dot_product_functor.h"

float vec_norm_avx256(const float* a_row, size_t N) noexcept;

float vec_normalize_avx256(float* a_row, size_t N) noexcept;

float dot_product_avx256(const float* __restrict__ a_row, const float* __restrict__ b_row, size_t N);

struct dot_product_avx256_t : public dot_product_t {
    inline float operator()(const float *const a_row, const float *const b_row, const size_t N) const noexcept final {
        return dot_product_avx256(a_row, b_row, N);
    }
};

#endif

#endif //FIRESTORM_DOT_PRODUCT_AVX256_H
