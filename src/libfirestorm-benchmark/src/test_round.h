//
// Created by sunside on 06.03.18.
//

#ifndef PROJECT_TEST_ROUND_H
#define PROJECT_TEST_ROUND_H

#include <memory>
#include <spdlog/spdlog.h>
#include <firestorm/engine/vector_ops/dot_product_functor.h>
#include <firestorm/engine/memory/chunk_manager.h>
#include <firestorm/engine/mapper/mapper_factory.h>
#include <firestorm/engine/mapper/dot_product_mapper_factory.h>
#include <firestorm/engine/worker/worker_t.h>
#include <firestorm/engine/types/vector_t.h>
#include <firestorm/engine/types/index_t.h>
#include <firestorm/engine/worker/worker_thread_coordinator.h>

namespace firestorm {

    const size_t BENCHMARK_NUM_DIMENSIONS = 2048;

    void run_test_round(const std::shared_ptr<spdlog::logger> &log, const dot_product_t &calculate, size_t repetitions,
                        float* result, const chunk_manager &chunkManager,
                        vector_ptr query,
                        score_t expected_best_score, size_t num_vectors);

    template<typename T>
    void run_test_round(const std::shared_ptr<spdlog::logger> &log, const size_t repetitions, float *const result,
                        const chunk_manager &chunkManager,
                        const vector_ptr query,
                        const score_t expected_best_score, size_t num_vectors) {

        static_assert(std::is_convertible<T *, dot_product_t *>::value, "Derived type must inherit dot_product_t as public");
        const T calculate{};

        run_test_round(log, calculate, repetitions, result, chunkManager, query, expected_best_score,
                       num_vectors);
    }

    void run_test_round_worker(const std::shared_ptr<spdlog::logger> &log, const std::shared_ptr<mapper_factory>& factory,
                               size_t repetitions, const worker_t &worker,
                               vector_ptr query,
                               const score_t& expected_best_score,
                               size_t num_vectors);

    template<typename T>
    void run_test_round_worker(const std::shared_ptr<spdlog::logger> &log, const size_t repetitions, const worker_t &worker,
                               const vector_ptr query,
                               const score_t& expected_best_score,
                               const size_t num_vectors) {

        const auto factory = std::make_shared<dot_product_mapper_factory<T>>();
        run_test_round_worker(log, factory, repetitions, worker, query, expected_best_score,
                              num_vectors);
    }

    void run_test_round_worker(const std::shared_ptr<spdlog::logger> &log, const std::shared_ptr<mapper_factory>& factory,
                               size_t repetitions,
                               const worker_thread_coordinator& coordinator, const vector_ptr& query,
                               const score_t& expected_best_score,
                               size_t num_vectors);

    template<typename T>
    void run_test_round_worker(const std::shared_ptr<spdlog::logger> &log, const size_t repetitions,
                               const worker_thread_coordinator& coordinator, const vector_ptr query,
                               const score_t& expected_best_score,
                               const size_t num_vectors) {

        const auto factory = std::make_shared<dot_product_mapper_factory<T>>();
        run_test_round_worker(log, factory, repetitions, coordinator, query, expected_best_score,
                              num_vectors);
    }

}

#endif //PROJECT_TEST_ROUND_H
