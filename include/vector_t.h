//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_VECTORT_H
#define FIRESTORM_VECTORT_H

#include <boost/align/aligned_alloc.hpp>
#include <bytes_t.h>

/// A single vector.
struct vector_t {
    vector_t(size_t dimensions)
        : dimensions(dimensions) {
        const bytes_t bytes = dimensions * sizeof(float);
        data = reinterpret_cast<float*>(boost::alignment::aligned_alloc(byte_alignment, bytes));
    }

    ~vector_t() {
        boost::alignment::aligned_free(data);
        data = nullptr;
    }

    /// Determines the alignment of the data field. Note that AVX requires 32 byte alignments.
    static const size_t byte_alignment = 32;

    /// The dimensionality of the query vector.
    const size_t dimensions;

    /// The actual data buffer.
    float* data;
};

#endif //FIRESTORM_VECTORT_H
