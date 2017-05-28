//
// Created by Markus on 28.05.2017.
//

#ifndef FIRESTORM_DOTPRODUCTVISITORAVX_H
#define FIRESTORM_DOTPRODUCTVISITORAVX_H

#include "Simd.h"
#include "ChunkVisitor.h"

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

            auto total = _mm256_set1_ps(0.0f);
            for (size_t i = 0; i < N; i += 32) {
                // Prefetch the next batch into L2 - saves around 40ms on 2 million 2048-float rows.
                _mm_prefetch(&a_row[i + 32 * 8], _MM_HINT_T1);
                _mm_prefetch(&b_row[i + 32 * 8], _MM_HINT_T1);

                // Load 32 floats per vector.
                const auto a0 = _mm256_load_ps(&a_row[i]);
                const auto b0 = _mm256_load_ps(&b_row[i]);
                const auto a1 = _mm256_load_ps(&a_row[i + 8]);
                const auto b1 = _mm256_load_ps(&b_row[i + 8]);
                const auto a2 = _mm256_load_ps(&a_row[i + 16]);
                const auto b2 = _mm256_load_ps(&b_row[i + 16]);
                const auto a3 = _mm256_load_ps(&a_row[i + 24]);
                const auto b3 = _mm256_load_ps(&b_row[i + 24]);

                // Do separate dot products.
                const auto c0 = _mm256_dp_ps(a0, b0, 0xff);
                const auto c1 = _mm256_dp_ps(a1, b1, 0xff);
                const auto c2 = _mm256_dp_ps(a2, b2, 0xff);
                const auto c3 = _mm256_dp_ps(a3, b3, 0xff);

                // Do separate partial sums.
                const auto p0 = _mm256_add_ps(c0, c1);
                const auto p1 = _mm256_add_ps(c2, c3);

                // Aggregate the partial sums and allocate to running total.
                const auto s = _mm256_add_ps(p0, p1);
                total = _mm256_add_ps(total, s);
            }

            alignas(32) float ptr[8];
            _mm256_store_ps(ptr, total);
            out_scores.data[vector] = ptr[0] + ptr[5];
        }
    };
};

#endif //FIRESTORM_DOTPRODUCTVISITORAVX_H
