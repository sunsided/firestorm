//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_VECTORT_H
#define FIRESTORM_VECTORT_H

#include <boost/align/aligned_alloc.hpp>
#include "firestorm/engine/types/bytes_t.h"

namespace firestorm {

    /// A single vector.
    struct vector_t {
        explicit vector_t(size_t dimensions) noexcept
                : dimensions(dimensions) {
            const bytes_t bytes = dimensions * sizeof(float);
            data = reinterpret_cast<float *>(boost::alignment::aligned_alloc(byte_alignment, bytes));
        }

        // Copy construction is forbidden.
        vector_t(const vector_t &other) = delete;

        // Move constructor.
        vector_t(vector_t &&other) noexcept
                : dimensions(other.dimensions), data(other.data) {
            other.data = nullptr;
        }

        ~vector_t() {
            if (data == nullptr) return;
            boost::alignment::aligned_free(data);
            data = nullptr;
        }

        /// Clears the content of the vector.
        inline void zero_out() {
            #pragma omp simd
            for (size_t i = 0; i < dimensions; ++i) {
                data[i] = 0;
            }
        }

        /// Determines the alignment of the data field. Note that AVX requires 32 byte alignments.
        static const size_t byte_alignment = 32;

        /// The dimensionality of the query vector.
        const size_t dimensions;

        /// The actual data buffer.
        float *data;
    };

}

#endif //FIRESTORM_VECTORT_H
