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
    /// Initializes a new memory chunk.
    ///
    /// \param index The index of the memory chunk.
    /// \param vectors The number of vectors stored by this chunk.
    /// \param dimensions The number of dimensions per vector.
    mem_chunk_t(const chunk_idx_t index, const size_t vectors, const size_t dimensions)
        : index(index), vectors(vectors), dimensions(dimensions) {
        const bytes_t bytes = vectors * dimensions * sizeof(float);
        data = reinterpret_cast<float*>(boost::alignment::aligned_alloc(byte_alignment, bytes));
    }

    ~mem_chunk_t() {
        boost::alignment::aligned_free(data);
    }

    /// Determines the alignment of the data field. Note that AVX requires 32 byte alignments.
    static const size_t byte_alignment = 32;

    /// The index of this chunk.
    const chunk_idx_t index;

    /// The number of vectors held by this chunk.
    const size_t vectors;

    /// The number of dimensions per vector.
    const size_t dimensions;

    /// The actual data field.
    float* data;
};

#endif //FIRESTORM_MEM_CHUNK_H
