//
// Created by sunside on 11.02.18.
//

#ifndef PROJECT_DOT_PRODUCT_SSE42_H
#define PROJECT_DOT_PRODUCT_SSE42_H

#if USE_SSE == 4

#include "dot_product_functor.h"

float dot_product_sse42(const float *__restrict__ a_row, const float *__restrict__ b_row, std::size_t N) noexcept;

struct dot_product_sse42_t final : public dot_product_t {
    explicit dot_product_sse42_t(const size_t N) noexcept
            : dot_product_t(N)
    {}

    float operator()(const float *__restrict__ a_row, const float *__restrict__ b_row) const noexcept final {
        return dot_product_sse42(a_row, b_row, N);
    }
};

float vec_norm_sse42(const float * a_row, size_t N) noexcept;

float vec_normalize_sse42(float* a_row, std::size_t N) noexcept;

#endif //USE_SSE

#endif //PROJECT_DOT_PRODUCT_SSE42_H
