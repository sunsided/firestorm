//
// Created by Markus on 11.02.2018.
//

#ifndef FIRESTORM_DOTPRODUCTMAPPER_H
#define FIRESTORM_DOTPRODUCTMAPPER_H

#include "ChunkMapper.h"
#include "firestorm/engine/ops/dot_product_functor.h"

namespace firestorm {

    template<typename Operation>
    class DotProductMapper final : public ChunkMapper {
        static_assert(std::is_convertible<Operation *, dot_product_t *>::value,
                      "Derived type must inherit dot_product_t as public");
    public:
        DotProductMapper() = default;
        ~DotProductMapper() final = default;

        std::any map(const mem_chunk_t &chunk, const vector_t &query) final {
            assert(chunk.dimensions == query.dimensions);

            std::vector<score_t> out_scores {chunk.vectors};

            const size_t N = query.dimensions;
            const auto query_vector = query.data;
            const auto ref_data = chunk.data;
            const size_t element_count =
                    chunk.vectors * chunk.dimensions; ///< The total number of float elements in the buffer.

            for (size_t start_idx = 0, vector_idx = 0; start_idx < element_count; start_idx += N, ++vector_idx) {
                const auto ref_vector = &ref_data[start_idx];
                const auto score = calculate(ref_vector, query_vector, N);

                out_scores[vector_idx] = score_t(static_cast<vector_idx_t >(vector_idx), score);
            }

            return out_scores;
        };

    private:
        const Operation calculate{};
    };
}

#endif //FIRESTORM_DOTPRODUCTMAPPER_H
