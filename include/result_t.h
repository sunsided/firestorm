//
// Created by Markus on 28.05.2017.
//

#ifndef FIRESTORM_RESULT_T_H
#define FIRESTORM_RESULT_T_H

#include "chunk_idx_t.h"
#include "vector_t.h"

struct result_t {
    result_t(const chunk_idx_t chunk_idx, const size_t num_vectors)
        : chunk(chunk_idx), vector(num_vectors) {}

    const chunk_idx_t chunk;
    const vector_t vector;
};

#endif //FIRESTORM_RESULT_T_H
