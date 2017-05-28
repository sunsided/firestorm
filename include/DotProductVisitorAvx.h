//
// Created by Markus on 28.05.2017.
//

#ifndef FIRESTORM_DOTPRODUCTVISITORAVX_H
#define FIRESTORM_DOTPRODUCTVISITORAVX_H

#include "Simd.h"
#include "ChunkVisitor.h"
#include "dot_product_avx256.h"

class DotProductVisitorAvx : public ChunkVisitor {
public:
    DotProductVisitorAvx() {}
    virtual ~DotProductVisitorAvx() {}

    void visit(const mem_chunk_t& chunk, const vector_t& query, const vector_t& out_scores) const override {
        assert(chunk.dimensions == query.dimensions);

        const size_t N = query.dimensions;
        const auto b_row = query.data;
        const size_t element_count = chunk.vectors * chunk.dimensions; ///< The total number of float elements in the buffer.

        for (size_t start_idx = 0, vector = 0; start_idx < element_count; start_idx += N, ++vector) {
            const auto a_row = &chunk.data[start_idx];

            out_scores.data[vector] = dot_product_avx256(a_row, b_row, N);
        }
    };
};

#endif //FIRESTORM_DOTPRODUCTVISITORAVX_H
