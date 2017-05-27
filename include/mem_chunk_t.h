//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_MEM_CHUNK_H
#define FIRESTORM_MEM_CHUNK_H

#include <boost/align/aligned_alloc.hpp>
#include <bytes_t.h>
#include "chunk_idx_t.h"

/// A memory chunk consisting of an arbitrary number of vectors.
struct mem_chunk_t {
    static const size_t byte_alignment = 32;
    const chunk_idx_t index;
    const bytes_t size;
    float* data;

    mem_chunk_t(chunk_idx_t index, bytes_t bytes)
        : index(index), size(bytes) {
        data = reinterpret_cast<float*>(boost::alignment::aligned_alloc(byte_alignment, bytes));
    }

    ~mem_chunk_t() {
        boost::alignment::aligned_free(data);
    }
};

#endif //FIRESTORM_MEM_CHUNK_H
