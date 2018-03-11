//
// Created by Markus on 11.02.2018.
//

#ifndef FIRESTORM_DOTPRODUCTMAPPER_H
#define FIRESTORM_DOTPRODUCTMAPPER_H

#include "mapper_t.h"
#include "firestorm/engine/vector_ops/dot_product_functor.h"

namespace firestorm {

    /// \brief A mapper that calculates the dot product of a query vector and every item in the chunk.
    template<typename Operation>
    class dot_product_mapper final : public mapper_t {
        static_assert(std::is_convertible<Operation *, dot_product_t *>::value,
                      "Derived type must inherit dot_product_t as public");
    public:
        dot_product_mapper() = default;
        ~dot_product_mapper() final = default;

        map_result_t map(const mem_chunk_t &chunk, const vector_t &query) const final {
            assert(chunk.dimensions == query.dimensions);

            std::vector<score_t> out_scores {chunk.vectors};

            const size_t N = query.dimensions;
            const auto chunk_idx = chunk.index;
            const auto query_vector = query.data;
            const auto ref_data = chunk.data;
            const size_t element_count = chunk.vectors * chunk.dimensions; ///< The total number of float elements in the buffer.

            for (size_t start_idx = 0, vector_idx = 0; start_idx < element_count; start_idx += N, ++vector_idx) {
                const auto ref_vector = &ref_data[start_idx];
                const auto dot_product = calculate(ref_vector, query_vector, N);

                const index_t index {chunk_idx, static_cast<vector_idx_t>(vector_idx)};
                const auto score = static_cast<score_value_t>(dot_product);
                out_scores[vector_idx] = score_t(index, score);
            }

            return out_scores;
        };

    private:
        const Operation calculate{};
    };
}

#endif //FIRESTORM_DOTPRODUCTMAPPER_H
