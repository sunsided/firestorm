//
// Created by sunside on 06.03.18.
//

#ifndef PROJECT_TEST_ROUND_H
#define PROJECT_TEST_ROUND_H

#include <memory>
#include <spdlog/spdlog.h>
#include <firestorm/engine/vector_ops/dot_product_functor.h>
#include <firestorm/engine/memory/ChunkManager.h>
#include <firestorm/engine/mapper/mapper_factory.h>
#include <firestorm/engine/mapper/dot_product_mapper_factory.h>
#include <firestorm/engine/Worker.h>
#include <firestorm/engine/types/vector_t.h>
#include <firestorm/engine/types/index_t.h>

namespace firestorm {

    const size_t BENCHMARK_NUM_DIMENSIONS = 2048;

    void run_test_round(const std::shared_ptr<spdlog::logger> &log, const dot_product_t &calculate, size_t repetitions,
                        float* result, const ChunkManager &chunkManager,
                        const vector_t &query,
                        score_t expected_best_score, size_t num_vectors);

    template<typename T>
    void run_test_round(const std::shared_ptr<spdlog::logger> &log, const size_t repetitions, float *const result,
                        const ChunkManager &chunkManager,
                        const vector_t &query,
                        const score_t expected_best_score, size_t num_vectors) {

        static_assert(std::is_convertible<T *, dot_product_t *>::value, "Derived type must inherit dot_product_t as public");
        const T calculate{};

        run_test_round(log, calculate, repetitions, result, chunkManager, query, expected_best_score,
                       num_vectors);
    }

    void run_test_round_worker(const std::shared_ptr<spdlog::logger> &log, const mapper_factory &factory,
                               size_t repetitions, const Worker &worker,
                               const vector_t &query,
                               score_t expected_best_score,
                               size_t num_vectors);

    template<typename T>
    void run_test_round_worker(const std::shared_ptr<spdlog::logger> &log, const size_t repetitions, const Worker &worker,
                               const vector_t &query,
                               const score_t expected_best_score,
                               const size_t num_vectors) {

        const dot_product_mapper_factory<T> factory{};
        run_test_round_worker(log, factory, repetitions, worker, query, expected_best_score,
                              num_vectors);
    }

    void run_test_round_worker(const std::shared_ptr<spdlog::logger> &log, const mapper_factory &factory,
                               size_t repetitions,
                               const std::vector<std::unique_ptr<Worker>> &workers, const vector_t &query,
                               score_t expected_best_score,
                               size_t num_vectors);

    template<typename T>
    void run_test_round_worker(const std::shared_ptr<spdlog::logger> &log, const size_t repetitions,
                               const std::vector<std::unique_ptr<Worker>> &workers, const vector_t &query,
                               const score_t expected_best_score,
                               const size_t num_vectors) {

        const dot_product_mapper_factory<T> factory{};
        run_test_round_worker(log, factory, repetitions, workers, query, expected_best_score,
                              num_vectors);
    }

}

#endif //PROJECT_TEST_ROUND_H
