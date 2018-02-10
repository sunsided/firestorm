//
// Created by Markus on 10.02.2018.
//

#ifndef FIRESTORM_NAIVEUNROLLEDVISITORAVX_H
#define FIRESTORM_NAIVEUNROLLEDVISITORAVX_H

#include "ChunkVisitor.h"
#include "dot_product_naive.h"

// TODO: Create type templated on the dot_product function

class NaiveUnrolledVisitorAvx : public ChunkVisitor {
public:
    NaiveUnrolledVisitorAvx() = default;
    virtual ~NaiveUnrolledVisitorAvx() = default;

    void visit(const mem_chunk_t& chunk, const vector_t& query, std::vector<float>& out_scores) const final {
        assert(chunk.dimensions == query.dimensions);

        const size_t N = query.dimensions;
        const auto query_vector = query.data;
        const size_t element_count = chunk.vectors * chunk.dimensions; ///< The total number of float elements in the buffer.

        for (size_t start_idx = 0, vector_idx = 0; start_idx < element_count; start_idx += N, ++vector_idx) {
            const auto ref_vector = &chunk.data[start_idx];
            out_scores[vector_idx] = dot_product_unrolled_8(ref_vector, query_vector, N);
        }
    };
};

#endif //FIRESTORM_NAIVEUNROLLEDVISITORAVX_H
