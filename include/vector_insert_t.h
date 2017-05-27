//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_VECTOR_INSERT_T_H
#define FIRESTORM_VECTOR_INSERT_T_H

#include "mem_chunk_t.h"
#include "context_t.h"

/// Data required for vector inserting.
struct vector_insert_t {
    vector_insert_t(const context_t &context, const mem_chunk_t vector) : context(context), vector(vector) {}

    /// The vector to insert
    const mem_chunk_t vector;
    /// Contextual information about the vector.
    const context_t context;
};

#endif //FIRESTORM_VECTOR_INSERT_T_H
