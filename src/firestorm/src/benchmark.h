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

vector_t create_query_vector(size_t NUM_DIMENSIONS);

void run_test_round(const std::shared_ptr<spdlog::logger> &log, const dot_product_t& calculate, size_t repetitions, float *const result, const ChunkManager &chunkManager,
                    const vector_t& query, bytes_t chunk_size,
                    size_t expected_best_idx, float expected_best_score, size_t num_vectors);

void run_test_round_worker(const std::shared_ptr<spdlog::logger> &log, const ChunkVisitor& visitor, size_t repetitions, const Worker &worker,
                           const vector_t& query,
                           size_t expected_best_idx, float expected_best_score, size_t num_vectors);

template <typename T>
void run_test_round(const std::shared_ptr<spdlog::logger> &log, const size_t repetitions, float *const result, const ChunkManager &chunkManager,
                    const vector_t& query, const bytes_t chunk_size,
                    const size_t expected_best_idx, float expected_best_score, size_t num_vectors) {

    static_assert(std::is_convertible<T*, dot_product_t*>::value, "Derived type must inherit dot_product_t as public");
    const T calculate {};

    run_test_round(log, calculate, repetitions, result, chunkManager, query, chunk_size, expected_best_idx, expected_best_score, num_vectors);
}

template <typename T>
void run_test_round_worker(const std::shared_ptr<spdlog::logger> &log, const size_t repetitions, const Worker &worker,
                           const vector_t& query,
                           const size_t expected_best_idx, const float expected_best_score, const size_t num_vectors) {

    const DotProductVisitor<T> visitor {};
    run_test_round_worker(log, visitor, repetitions, worker, query, expected_best_idx, expected_best_score, num_vectors);
}

#endif //PROJECT_BENCHMARK_H
