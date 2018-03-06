//
// Created by Markus on 11.02.2018.
//

#ifndef FIRESTORM_DOTPRODUCTVISITOR_H
#define FIRESTORM_DOTPRODUCTVISITOR_H

#include "ChunkMapper.h"
#include "dot_product_functor.h"

namespace firestorm {

    class DotProductMapper_ : public ChunkMapper {
    public:
        DotProductMapper_() = default;

        virtual ~DotProductMapper_() = default;

        /// Gets the accumulated scores.
        /// \return The scores.
        virtual std::vector<score_t> scores() const = 0;
    };

    template<typename Operation>
    class DotProductMapper final : public DotProductMapper_ {
        static_assert(std::is_convertible<Operation *, dot_product_t *>::value,
                      "Derived type must inherit dot_product_t as public");

    public:
        DotProductMapper() = default;

        ~DotProductMapper() final = default;

        void map_prepare() final {
            out_scores.clear();
        };

        virtual void map(const mem_chunk_t &chunk, const vector_t &query) final {
            assert(chunk.dimensions == query.dimensions);

            const size_t N = query.dimensions;
            const auto query_vector = query.data;
            const auto ref_data = chunk.data;
            const size_t element_count =
                    chunk.vectors * chunk.dimensions; ///< The total number of float elements in the buffer.

            for (size_t start_idx = 0, vector_idx = 0; start_idx < element_count; start_idx += N, ++vector_idx) {
                const auto ref_vector = &ref_data[start_idx];
                const auto score = calculate(ref_vector, query_vector, N);

                out_scores.emplace_back(static_cast<vector_idx_t >(vector_idx), score);
            }
        };

        virtual void map_done() final {};

        virtual void combine(const ChunkMapper &other) final {
            // TODO implement result combination
            auto other_dot = static_cast<const DotProductMapper &>(other);
            for (auto result : other_dot.out_scores) {
                out_scores.push_back(result);
            }
        }

        /// Gets the accumulated scores.
        /// \return The scores.
        virtual std::vector<score_t> scores() const final {
            return out_scores;
        }

    private:
        const Operation calculate{};
        std::vector<score_t> out_scores;
    };

}

#endif //FIRESTORM_DOTPRODUCTVISITOR_H
