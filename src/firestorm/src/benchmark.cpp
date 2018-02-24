//
// Created by sunside on 24.02.18.
//

#include <memory>
#include <random>
#include <spdlog/spdlog.h>
#include <firestorm/vector_t.h>
#include <firestorm/dot_product_naive.h>
#include "benchmark.h"

using namespace std;

vector_t create_query_vector(const size_t NUM_DIMENSIONS) {
    const auto seed = 0L;
    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(0.0f, 2.0f);
    auto random = std::bind(distribution, generator);

    // Create a simple query vector
    vector_t query {NUM_DIMENSIONS};
    for (size_t i = 0; i < NUM_DIMENSIONS; ++i) {
        query.data[i] = random();
    }

    vec_normalize_naive(query.data, query.dimensions);
    return query;
}

void run_test_round(const std::shared_ptr<spdlog::logger> &log, const dot_product_t& calculate, const size_t repetitions, float *const result, const ChunkManager &chunkManager,
                    const vector_t& query, const bytes_t chunk_size,
                    const size_t expected_best_idx, float expected_best_score, size_t num_vectors) {

    const auto vectors_per_chunk = chunk_size / (NUM_DIMENSIONS * sizeof(float));
    auto total_duration_ms = static_cast<size_t>(0);

    for (size_t repetition = 0; repetition < repetitions; ++repetition) {
        auto start_time = std::chrono::_V2::system_clock::now();

        // Keep track of the total sum for validation.
        auto best_match = 0.0f;
        auto best_match_idx = static_cast<size_t>(0);

        chunk_idx_t current_chunk = 0;
        auto chunk = chunkManager.get_ro(current_chunk);
        auto query_vector = query.data;

        auto float_offset = 0;
        auto remaining_vectors_per_chunk = vectors_per_chunk;

        // Run over all vectors ...
        for (size_t vector_idx = 0; vector_idx < num_vectors; ++vector_idx) {

            if (remaining_vectors_per_chunk == 0) {
                current_chunk += 1;
                chunk = chunkManager.get_ro(current_chunk);

                float_offset = 0;
                remaining_vectors_per_chunk = vectors_per_chunk;
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
                best_match_idx = vector_idx;
            }
        }

        auto end_time = std::chrono::_V2::system_clock::now();
        auto local_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        total_duration_ms += local_duration_ms;

        auto local_vectors_per_second = static_cast<float>(num_vectors) * MS_TO_S / static_cast<float>(local_duration_ms);
        log->debug("- Round {}/{} matched {} at {} (expected {} at {}); took {} ms ({} vectors/s)",
                   repetition + 1, repetitions,
                   best_match, best_match_idx,
                   expected_best_score, expected_best_idx,
                   local_duration_ms, local_vectors_per_second);
    }

    const size_t total_num_vectors = repetitions * num_vectors;
    auto vectors_per_second = static_cast<float>(total_num_vectors) * MS_TO_S / static_cast<float>(total_duration_ms);
    log->info("- Processed {} vectors in {} ms ({} vectors/s)",
              total_num_vectors, total_duration_ms, vectors_per_second);
}

void run_test_round_worker(const std::shared_ptr<spdlog::logger> &log, const ChunkVisitor& visitor, const size_t repetitions, const Worker &worker,
                           const vector_t& query,
                           const size_t expected_best_idx, const float expected_best_score, const size_t num_vectors) {
    auto total_duration_ms = static_cast<size_t>(0);

    for (size_t repetition = 0; repetition < repetitions; ++repetition) {
        auto start_time = std::chrono::_V2::system_clock::now();
        auto results = worker.create_result_buffer();

        // Keep track of the total sum for validation.
        auto best_match = 0.0f;
        auto best_match_idx = static_cast<size_t>(0);

        worker.accept(visitor, query, results);

        auto end_time = std::chrono::_V2::system_clock::now();
        auto local_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        total_duration_ms += local_duration_ms;

        // TODO: This should eventually be part of the worker, otherwise we're going through the lists twice.
        for (auto chunk_result : results) {
            for (size_t vector_idx = 0; vector_idx < num_vectors; ++vector_idx) {
                const auto score = chunk_result.second->scores[vector_idx];
                if (score > best_match) {
                    best_match = score;
                    best_match_idx = vector_idx;
                }
            }
        }

        auto local_vectors_per_second = static_cast<float>(num_vectors) * MS_TO_S / static_cast<float>(local_duration_ms);
        log->debug("- Round {}/{} matched {} at {} (expected {} at {}); took {} ms ({} vectors/s)",
                   repetition + 1, repetitions,
                   best_match, best_match_idx,
                   expected_best_score, expected_best_idx,
                   local_duration_ms, local_vectors_per_second);
    }

    const size_t total_num_vectors = repetitions * num_vectors;
    auto vectors_per_second = static_cast<float>(total_num_vectors) * MS_TO_S / static_cast<float>(total_duration_ms);
    log->info("- Processed {} vectors in {} ms ({} vectors/s)",
              total_num_vectors, total_duration_ms, vectors_per_second);
}
