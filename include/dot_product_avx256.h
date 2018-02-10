//
// Created by Markus on 28.05.2017.
//

#ifndef FIRESTORM_DOT_PRODUCT_AVX256_H
#define FIRESTORM_DOT_PRODUCT_AVX256_H

#if USE_AVX2 || USE_AVX

#include <memory>

float vec_norm_avx256(const float *const a_row, const size_t N);

float vec_normalize_avx256(float *const a_row, const size_t N);

float dot_product_avx256(const float *const a_row, const float *const b_row, const size_t N);

#endif

#endif //FIRESTORM_DOT_PRODUCT_AVX256_H
