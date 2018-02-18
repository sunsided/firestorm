//
// Created by sunside on 11.02.18.
//

#include <gtest/gtest.h>
#include <firestorm/vector_t.h>
#include <firestorm/dot_product_sse42.h>
#include <VectorNorm.h>
#include <DotProduct.h>

using namespace std;

namespace {

    TEST_P(VectorNorm, SSE42_Normalize) {
        // arrange
        const auto test_data = GetParam();
        const auto& vector = *get<0>(test_data).get();
        const auto referenceNorm = get<1>(test_data);

        // act
        auto norm = vec_norm_sse42(vector.data, vector.dimensions);
        auto normBefore = vec_normalize_sse42(vector.data, vector.dimensions);
        auto normAfter = vec_norm_sse42(vector.data, vector.dimensions);

        // assert
        ASSERT_FLOAT_EQ(norm, normBefore);
        ASSERT_FLOAT_EQ(normBefore, referenceNorm);
        if (normBefore > 0.0f) {
            ASSERT_FLOAT_EQ(normAfter, 1.0f);
        }
        else {
            ASSERT_FLOAT_EQ(normAfter, 0.0f);
        }
    }

    TEST_F(DotProduct, SSE42) {
        // arrange
        dot_product_sse42_t dot {};

        // act
        auto result = dot(vector_a.data, vector_b.data, vector_a.dimensions);

        // assert
        ASSERT_FLOAT_EQ(result, this->result);
    }
}