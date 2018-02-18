//
// Created by sunside on 11.02.18.
//

#ifndef FIRESTORM_DOT_PRODUCT_FUNCTOR_H
#define FIRESTORM_DOT_PRODUCT_FUNCTOR_H

#include <cstddef>

const float FLOAT_ZERO_EPSILON = 1e-5f;

struct dot_product_t {
    virtual float operator()(const float *__restrict__ a_row, const float *__restrict__ b_row, std::size_t N) const noexcept = 0;
};

#endif //FIRESTORM_DOT_PRODUCT_FUNCTOR_H
