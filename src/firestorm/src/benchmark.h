//
// Created by sunside on 24.02.18.
//

#ifndef PROJECT_BENCHMARK_H
#define PROJECT_BENCHMARK_H

#include <random>
#include <chrono>
#include <memory>
#include <spdlog/spdlog.h>
#include <firestorm/ChunkManager.h>
#include <firestorm/dot_product_functor.h>
#include <firestorm/vector_t.h>
#include <firestorm/Worker.h>
#include <firestorm/DotProductVisitor.h>

const auto MS_TO_S = 1000.0F;

const size_t NUM_DIMENSIONS = 2048;
#if USE_AVX
const size_t NUM_VECTORS = 100000;
#else
const size_t NUM_VECTORS = 5000;
#endif

vector_t create_query_vector(const std::shared_ptr<spdlog::logger> &log, size_t NUM_DIMENSIONS);

template <typename T>
void run_test_round(const std::shared_ptr<spdlog::logger> &log, const size_t repetitions, float *const result, const ChunkManager &chunkManager,
                    const vector_t& query, const bytes_t chunk_size, const size_t expected_best_idx, float expected_best_score) {

    static_assert(std::is_convertible<T*, dot_product_t*>::value, "Derived type must inherit dot_product_t as public");
    const T calculate {};

    const auto vectors_per_chunk = chunk_size / (NUM_DIMENSIONS * sizeof(float));
    auto duration_ms = static_cast<size_t>(0);

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
        for (size_t vector_idx = 0; vector_idx < NUM_VECTORS; ++vector_idx) {

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
        duration_ms += local_duration_ms;

        auto local_vectors_per_second = static_cast<float>(NUM_VECTORS) * MS_TO_S / static_cast<float>(local_duration_ms);
        log->debug("- Round {}/{} matched {} at {} (expected {} at {}); took {} ms ({} vectors/s)",
                   repetition + 1, repetitions,
                   best_match, best_match_idx,
                   expected_best_score, expected_best_idx,
                   local_duration_ms, local_vectors_per_second);
    }

    const size_t num_vectors = repetitions * NUM_VECTORS;
    auto vectors_per_second = static_cast<float>(num_vectors) * MS_TO_S / static_cast<float>(duration_ms);
    log->info("- Processed {} vectors in {} ms ({} vectors/s)",
              num_vectors, duration_ms, vectors_per_second);
}

template <typename T>
void run_test_round_worker(const std::shared_ptr<spdlog::logger> &log, const size_t repetitions, const Worker &worker,
                           const vector_t& query, const size_t expected_best_idx, const float expected_best_score) {

    const DotProductVisitor<T> visitor {};
    auto duration_ms = static_cast<size_t>(0);

    for (size_t repetition = 0; repetition < repetitions; ++repetition) {
        auto start_time = std::chrono::_V2::system_clock::now();
        auto results = worker.create_result_buffer();

        // Keep track of the total sum for validation.
        auto best_match = 0.0f;
        auto best_match_idx = static_cast<size_t>(0);

        worker.accept(visitor, query, results);

        auto end_time = std::chrono::_V2::system_clock::now();
        auto local_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        duration_ms += local_duration_ms;

        // TODO: This should eventually be part of the worker, otherwise we're going through the lists twice.
        for (auto chunk_result : results) {
            for (size_t vector_idx = 0; vector_idx < NUM_VECTORS; ++vector_idx) {
                const auto score = chunk_result.second->scores[vector_idx];
                if (score > best_match) {
                    best_match = score;
                    best_match_idx = vector_idx;
                }
            }
        }

        auto local_vectors_per_second = static_cast<float>(NUM_VECTORS) * MS_TO_S / static_cast<float>(local_duration_ms);
        log->debug("- Round {}/{} matched {} at {} (expected {} at {}); took {} ms ({} vectors/s)",
                   repetition + 1, repetitions,
                   best_match, best_match_idx,
                   expected_best_score, expected_best_idx,
                   local_duration_ms, local_vectors_per_second);
    }

    const size_t num_vectors = repetitions * NUM_VECTORS;
    auto vectors_per_second = static_cast<float>(num_vectors) * MS_TO_S / static_cast<float>(duration_ms);
    log->info("- Processed {} vectors in {} ms ({} vectors/s)",
              num_vectors, duration_ms, vectors_per_second);
}

#endif //PROJECT_BENCHMARK_H
