//
// Created by sunside on 10.02.18.
//

#ifndef FIRESTORM_DOT_PRODUCT_NAIVE_H
#define FIRESTORM_DOT_PRODUCT_NAIVE_H

struct dot_product_naive_t {
    float operator()(const float *__restrict__ a_row, const float *__restrict__ b_row, size_t N) const;
};

float dot_product_unrolled_8(const float *__restrict__ a_row, const float *__restrict__ b_row, size_t N);

struct dot_product_unrolled_8_t {
    inline float operator()(const float *__restrict__ a_row, const float *__restrict__ b_row, size_t N) const {
        return dot_product_unrolled_8(a_row, b_row, N);
    }
};

float vec_norm_naive(const float* a_row, size_t N);

float vec_normalize_naive(float* a_row, size_t N);

#endif //FIRESTORM_DOT_PRODUCT_NAIVE_H
