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

/// Runs algorithm benchmarks and prints out the results.
/// \param log The logger to write to.
/// \param num_vectors The number of vectors to test.
void run_benchmark(const std::shared_ptr<spdlog::logger> &log, size_t num_vectors);

#endif //PROJECT_BENCHMARK_H
