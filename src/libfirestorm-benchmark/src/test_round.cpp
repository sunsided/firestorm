//
// Created by sunside on 06.03.18.
//

#include <chrono>
#include "test_round.h"

using namespace spdlog;
using namespace std;

namespace firestorm {

    void run_test_round(const shared_ptr<spdlog::logger> &log, const dot_product_t &calculate, const size_t repetitions,
                        float *const result, const ChunkManager &chunkManager,
                        const vector_t &query,
                        const index_t expected_best_idx, float expected_best_score, size_t num_vectors) {

        auto total_duration_ms = static_cast<size_t>(0);
        auto total_num_vectors = static_cast<size_t>(0);

        for (size_t repetition = 0; repetition < repetitions; ++repetition) {
            auto start_time = chrono::_V2::system_clock::now();

            // Keep track of the total sum for validation.
            auto best_match = 0.0f;
            index_t best_match_idx {0, 0};

            chunk_idx_t current_chunk = 0;
            auto chunk = chunkManager.get_ro(current_chunk);
            auto remaining_vectors_per_chunk = chunk->vectors;
            total_num_vectors += remaining_vectors_per_chunk;

            auto query_vector = query.data;
            auto float_offset = 0;


            // Run over all vectors ...
            for (size_t vector_idx = 0; vector_idx < num_vectors; ++vector_idx) {

                if (remaining_vectors_per_chunk == 0) {
                    current_chunk += 1;
                    chunk = chunkManager.get_ro(current_chunk);

                    remaining_vectors_per_chunk = chunk->vectors;
                    total_num_vectors += remaining_vectors_per_chunk;

                    float_offset = 0;
                }

                auto ref_vector = &chunk->data[float_offset];

                --remaining_vectors_per_chunk;
                float_offset += NUM_DIMENSIONS;

                // Calculate the dot product of the 2048-element vector
                static_assert((NUM_DIMENSIONS & 31) == 0, "Vector length must be a multiple of 32 elements.");
                const auto dot_product = calculate(ref_vector, query_vector, NUM_DIMENSIONS);

                result[vector_idx] = dot_product;

                if (dot_product > best_match) {
                    best_match = dot_product;
                    best_match_idx = {chunk->index, static_cast<vector_idx_t>(vector_idx)};
                }
            }

            auto end_time = chrono::_V2::system_clock::now();
            auto local_duration_ms = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
            total_duration_ms += local_duration_ms;

            auto local_vectors_per_second =
                    static_cast<float>(num_vectors) * MS_TO_S / static_cast<float>(local_duration_ms);
            log->debug("- Round {}/{} matched {} at {}.{} (expected {} at {}.{}); took {} ms ({} vectors/s)",
                       repetition + 1, repetitions,
                       best_match, best_match_idx.chunk(), best_match_idx.vector_index(),
                       expected_best_score, expected_best_idx.chunk(), expected_best_idx.vector_index(),
                       local_duration_ms, local_vectors_per_second);
        }

        auto vectors_per_second =
                static_cast<float>(total_num_vectors) * MS_TO_S / static_cast<float>(total_duration_ms);
        log->info("- Processed {} vectors in {} ms ({} vectors/s)",
                  total_num_vectors, total_duration_ms, vectors_per_second);
    }

}