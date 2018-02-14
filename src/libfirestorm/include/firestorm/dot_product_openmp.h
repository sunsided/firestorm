//
// Created by sunside on 11.02.18.
//

#ifndef FIRESTORM_DOT_PRODUCT_OPENMP_H
#define FIRESTORM_DOT_PRODUCT_OPENMP_H

#if OPENMP

#include "dot_product_functor.h"

float dot_product_openmp(const float *__restrict__ a_row, const float *__restrict__ b_row, std::size_t N) noexcept;

struct dot_product_openmp_t final : public dot_product_t {
    float operator()(const float *__restrict__ a_row, const float *__restrict__ b_row, std::size_t N) const noexcept final {
        return dot_product_openmp(a_row, b_row, N);
    }
};

float vec_norm_openmp(const float* a_row, std::size_t N) noexcept;

float vec_normalize_openmp(float* a_row, std::size_t N) noexcept;

#endif //OPENMP

#endif //FIRESTORM_DOT_PRODUCT_NAIVE_H
