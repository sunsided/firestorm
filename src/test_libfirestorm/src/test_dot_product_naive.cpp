//
// Created by sunside on 11.02.18.
//

#include <gtest/gtest.h>
#include <firestorm/vector_t.h>
#include <firestorm/test_dot_product_naive.h>

using namespace std;

namespace {

    TEST(DotProduct, Naive_NormZero) {
        // arrange
        const size_t N = 64;
        const float referenceNorm = 0.0f;
        vector_t vector {N};
        vector.data[0] = referenceNorm;

        // act
        auto norm = vec_norm_naive(vector.data, N);

        // assert
        EXPECT_EQ(norm, referenceNorm);
    }

    TEST(DotProduct, Naive_NormOne) {
        // arrange
        const size_t N = 64;
        const float referenceNorm = 1.0f;
        vector_t vector {N};
        vector.data[0] = referenceNorm;

        // act
        auto norm = vec_norm_naive(vector.data, N);

        // assert
        EXPECT_EQ(norm, referenceNorm);
    }

}