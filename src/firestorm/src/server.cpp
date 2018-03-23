//
// Created by sunside on 21.03.18.
//

#include <any>
#include <cassert>
#include <thread>
#include <vector>
#include <firestorm/engine/executor/local_executor_t.h>
#include <firestorm/benchmark/vector_generator.h>
#include <firestorm/engine/memory/chunk_manager.h>
#include <firestorm/engine/job/job_coordinator.h>
#include <firestorm/engine/mapreduce/mapper/dot_product_mapper_factory.h>
#include <firestorm/engine/mapreduce/combiner/keep_all_combiner_factory.h>
#include <firestorm/engine/mapreduce/reducer/keep_all_reducer_factory.h>
#include <firestorm/engine/vector_ops/dot_product_naive.h>
#include <firestorm/logging/logger_t.h>
#include "server.h"

using namespace firestorm;

const size_t default_worker_count = 8;

const size_t BENCHMARK_NUM_DIMENSIONS = 2048U;
const size_t BENCHMARK_NUM_VECTORS = 10000;
const size_t BENCHMARK_VECTOR_SEED = 1337;

vector_ptr generate_vectors(const logger_t& log,
                      const std::shared_ptr<chunk_manager>& chunkManager, size_t target_chunk_size,
                      const std::shared_ptr<worker_thread_coordinator>& wtc) {
    vector_generator vec_gen{BENCHMARK_NUM_DIMENSIONS, BENCHMARK_VECTOR_SEED};

    const size_t num_vectors_per_chunk = target_chunk_size / (BENCHMARK_NUM_DIMENSIONS * sizeof(float));
    assert(target_chunk_size % (sizeof(float) * BENCHMARK_NUM_DIMENSIONS) == 0);

    std::shared_ptr<mem_chunk_t> chunk = nullptr;
    auto remaining_chunk_size = 0_B;    // number of remaining bytes in the current chunk
    size_t float_offset = 0;            // index into the current buffer, counts floats

    for (size_t j = 0; j < BENCHMARK_NUM_VECTORS; ++j) {
        // Initial condition, also reached during runtime:
        // If one memory chunk is "full", allocate another one.
        if (remaining_chunk_size == 0_B) {
            log->debug("Allocating chunk.");

            chunk = chunkManager->allocate(num_vectors_per_chunk, BENCHMARK_NUM_DIMENSIONS);
            assert(chunk != nullptr);

            remaining_chunk_size = target_chunk_size;
            float_offset = 0;

            // Assign the chunk
            wtc->assign_chunk(chunk);
        }

        // Some progress printing.
        if (j % 2500 == 0) {
            log->info("- {}/{}", j, BENCHMARK_NUM_VECTORS);
        }

        auto a = &chunk->data[float_offset];
        assert(a != nullptr);

        assert(float_offset < BENCHMARK_NUM_VECTORS * BENCHMARK_NUM_DIMENSIONS);
        assert(remaining_chunk_size >= sizeof(float) * BENCHMARK_NUM_DIMENSIONS);

        remaining_chunk_size -= (sizeof(float) * BENCHMARK_NUM_DIMENSIONS);
        float_offset += BENCHMARK_NUM_DIMENSIONS;

        // Create a vector in-place.
        vec_gen.fill_vector(a);
    }
    log->info("Done allocating chunks.");

    // Return a query vector.
    return vec_gen.create_vector(0L);
}

int run_server(logger_t log) {
    // The instance identifier uniquely identifies this process.
    auto instance_identifier = std::make_shared<instance_identifier_t>();

    // The worker thread coordinator is used for local (in-process) work distribution.
    auto worker_count = std::thread::hardware_concurrency();
    if (worker_count == 0) worker_count = default_worker_count;
    auto wtc = std::make_shared<worker_thread_coordinator>(worker_count); // TODO: This one currently does synchronous operations.

    // The local executor handles the local work coordination.
    auto local_executor = std::make_shared<local_executor_t>(instance_identifier, wtc);

    // The job coordinator takes care of dispatching jobs and collecting their results.
    std::vector<executor_ptr> executors;
    executors.push_back(local_executor);
    job_coordinator coordinator{executors};

    // The chunk manager manages the memory locations for our vectors.
    auto cm = std::make_shared<chunk_manager>(); // TODO: Actually it should be singleton and passed by-ref
    const size_t target_chunk_size = 32_MB; // TODO: This should be a property of the manager itself

    // Let's generate some vectors.
    const auto query_vector = generate_vectors(log, cm, target_chunk_size, wtc);

    // We need a mapper and reducer.
    auto mapper_factory = std::make_shared<dot_product_mapper_factory<dot_product_unrolled_8_t>>();
    auto combiner_factory = std::make_shared<keep_all_combiner_factory>();
    auto reducer_factory = std::make_shared<keep_all_reducer_factory>();

    // We can now perform a query.
    auto future = coordinator.query(mapper_factory, combiner_factory, reducer_factory, query_vector);

    const auto& job_result = future.get();
    assert(job_result.status().completion_type() == job_completion::succeeded);

    auto maybe_result = job_result.result();
    assert(maybe_result);

    auto results = std::any_cast<std::vector<score_t>>(*maybe_result);

    // With keep_all, we will see more results than we have actual vectors (depending on the number of
    // vectors), since this also keeps all unassigned slots in a chunk.
    const auto expected_num_vectors = (cm->size() * target_chunk_size) / (sizeof(float) * BENCHMARK_NUM_DIMENSIONS);
    log->info("Vector results: {}. Expected results: {}", results.size(), expected_num_vectors);
    assert(results.size() == expected_num_vectors);

    // Since keep_all only saves every result, let's determine the best match now.
    score_t best_result;
    score_t worst_result;
    for(const auto& result : results) {
        if (result.score() > best_result || best_result.invalid()) {
            best_result = result;
        }
        if (result.score() < worst_result || worst_result.invalid()) {
            worst_result = result;
        }
    }
    log->info("Best match found at chunk {}, index {} with a score of {}.",
              best_result.index().chunk(), best_result.index().vector_index(), best_result.score());
    log->info("Worst match found at chunk {}, index {} with a score of {}.",
              worst_result.index().chunk(), worst_result.index().vector_index(), worst_result.score());

    return 0;
}