//
// Created by sunside on 11.02.18.
//

#include <gtest/gtest.h>
#include <firestorm/vector_t.h>
#include <firestorm/dot_product_naive.h>
#include <VectorNorm.h>

using namespace std;

namespace {

    TEST_P(VectorNorm, Naive_Norm) {
        // arrange
        const auto test_data = GetParam();
        const auto& vector = *get<0>(test_data).get();
        const auto referenceNorm = get<1>(test_data);

        // act
        auto norm = vec_norm_naive(vector.data, vector.dimensions);

        // assert
        ASSERT_FLOAT_EQ(norm, referenceNorm);
    }

    TEST_P(VectorNorm, Naive_Normalize) {
        // arrange
        const auto test_data = GetParam();
        const auto& vector = *get<0>(test_data).get();
        const auto referenceNorm = get<1>(test_data);

        // act
        auto normBefore = vec_normalize_naive(vector.data, vector.dimensions);
        auto normAfter = vec_norm_naive(vector.data, vector.dimensions);

        // assert
        ASSERT_FLOAT_EQ(normBefore, referenceNorm);
        if (normBefore > 0.0f) {
            ASSERT_FLOAT_EQ(normAfter, 1.0f);
        }
        else {
            ASSERT_FLOAT_EQ(normAfter, 0.0f);
        }
    }
}