//
// Created by Markus on 28.05.2017.
//

#ifndef FIRESTORM_DOT_PRODUCT_AVX256_H
#define FIRESTORM_DOT_PRODUCT_AVX256_H

#if AVX_VERSION

#include <memory>
#include "dot_product_functor.h"

float vec_norm_avx256(const float* a_row, std::size_t N) noexcept;

float vec_normalize_avx256(float* a_row, std::size_t N) noexcept;

float dot_product_avx256(const float* __restrict__ a_row, const float* __restrict__ b_row, std::size_t N) noexcept;

struct dot_product_avx256_t final : public dot_product_t {
    inline float operator()(const float *const a_row, const float *const b_row, const std::size_t N) const noexcept final {
        return dot_product_avx256(a_row, b_row, N);
    }
};

#endif //AVX_VERSION

#endif //FIRESTORM_DOT_PRODUCT_AVX256_H
