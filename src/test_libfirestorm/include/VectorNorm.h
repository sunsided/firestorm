//
// Created by sunside on 17.02.18.
//

#ifndef PROJECT_VECTORNORM_H
#define PROJECT_VECTORNORM_H

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <tuple>
#include <firestorm/vector_t.h>

std::tuple<std::shared_ptr<vector_t>, float> create_vector_norm_pair(float norm, size_t dimensions);

class VectorNorm : public ::testing::TestWithParam<std::tuple<std::shared_ptr<vector_t>, float>>
{
};

#endif //PROJECT_VECTORNORM_H
