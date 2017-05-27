//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_VECTORT_H
#define FIRESTORM_VECTORT_H

#include <boost/align/aligned_alloc.hpp>
#include <bytes_t.h>

/// A single vector.
struct vector_t {
    static const size_t byte_alignment = 32;
    const size_t elements;
    float* data;

    vector_t(size_t elements)
        : elements(elements) {
        const bytes_t bytes = elements * sizeof(float);
        data = reinterpret_cast<float*>(boost::alignment::aligned_alloc(byte_alignment, bytes));
    }

    ~vector_t() {
        boost::alignment::aligned_free(data);
        data = nullptr;
    }
};

#endif //FIRESTORM_VECTORT_H
