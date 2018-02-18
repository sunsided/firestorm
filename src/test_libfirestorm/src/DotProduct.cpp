//
// Created by sunside on 18.02.18.
//

#include <DotProduct.h>

DotProduct::DotProduct()
    : vector_a{32}, vector_b{32}, result(1.5)
{
    for (int i = 0; i < vector_a.dimensions; ++i) {
        vector_a.data[i] = 0.5;
    }

    vector_b.data[0] = 2;
    vector_b.data[1] = -2;

    vector_b.data[31] = 3;
}