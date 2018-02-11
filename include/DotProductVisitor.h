//
// Created by Markus on 11.02.2018.
//

#ifndef FIRESTORM_DOTPRODUCTVISITOR_H
#define FIRESTORM_DOTPRODUCTVISITOR_H

#include "ChunkVisitor.h"
#include "dot_product_functor.h"

template <typename T>
class DotProductVisitor : public ChunkVisitor {
    static_assert(std::is_convertible<T*, dot_product_t*>::value, "Derived type must inherit dot_product_t as public");

public:
    DotProductVisitor() = default;
    virtual ~DotProductVisitor() = default;

    void visit(const mem_chunk_t& chunk, const vector_t& query, std::vector<float>& out_scores) const final {
        assert(chunk.dimensions == query.dimensions);

        const size_t N = query.dimensions;
        const auto query_vector = query.data;
        const auto ref_data = chunk.data;
        const size_t element_count = chunk.vectors * chunk.dimensions; ///< The total number of float elements in the buffer.

        for (size_t start_idx = 0, vector_idx = 0; start_idx < element_count; start_idx += N, ++vector_idx) {
            const auto ref_vector = &chunk.data[start_idx];
            out_scores[vector_idx] = calculate(ref_vector, query_vector, N);
        }
    };

private:
    const T calculate{};
};

#endif //FIRESTORM_DOTPRODUCTVISITOR_H
