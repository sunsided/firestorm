//
// Created by sunside on 17.02.18.
//

#include <cmath>
#include "VectorNorm.h"

using namespace std;

static const size_t NUM_DIMENSIONS = 64;

INSTANTIATE_TEST_CASE_P(EnsureVectorNorms,
                        VectorNorm,
                        ::testing::Values(
                                create_vector_norm_pair(0.0f, 64),
                                create_vector_norm_pair(1.0f, 64),
                                create_vector_norm_pair(2.0f, 64),
                                create_vector_norm_pair(17.321f, 64)
                        ));

static inline shared_ptr<vector_t> create_vector(const float norm, const size_t n) {
    auto vector = make_shared<vector_t>(n);
    vector->zero_out();

    const auto sn = norm;
    vector->data[n - 1] = sn;

    return vector;
}

tuple<shared_ptr<vector_t>, float> create_vector_norm_pair(const float norm, const size_t dimensions) {
    return make_tuple(create_vector(norm, dimensions), norm);
}
