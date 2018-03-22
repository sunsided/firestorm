//
// Created by sunside on 24.02.18.
//

#ifndef PROJECT_BENCHMARK_H
#define PROJECT_BENCHMARK_H

#include <random>
#include <chrono>
#include <memory>
#include <firestorm/engine/memory/ChunkManager.h>
#include <firestorm/engine/vector_ops/dot_product_functor.h>
#include <firestorm/engine/types/vector_t.h>
#include <firestorm/engine/worker/worker_t.h>
#include <firestorm/engine/mapper/dot_product_mapper.h>
#include <firestorm/logging/logger_t.h>

namespace firestorm {

    /// Runs algorithm benchmarks and prints out the results.
    /// \param log The logger to write to.
    /// \param num_vectors The number of vectors to test.
    /// \param target_chunk_size The chunk size to work with.
    /// \param num_workers The number of concurrent workers to use. If unset, use native system concurrency.
    void run_benchmark(const logger_t &log, size_t num_vectors, size_t target_chunk_size = 32_MB,
                       boost::optional<size_t> num_workers = boost::none);

}

#endif //PROJECT_BENCHMARK_H
