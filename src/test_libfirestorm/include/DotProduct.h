//
// Created by sunside on 18.02.18.
//

#ifndef PROJECT_DOTPRODUCT_H
#define PROJECT_DOTPRODUCT_H

#include <gtest/gtest.h>
#include <firestorm/engine/vector_t.h>

class DotProduct : public ::testing::Test
{
public:
    firestorm::vector_t vector_a;
    firestorm::vector_t vector_b;
    float result;

    DotProduct();
};

#endif //PROJECT_DOTPRODUCT_H
