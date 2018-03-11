//
// Created by sunside on 24.02.18.
//

#include <thread>
#include <random>
#include <algorithm>

#include <spdlog/spdlog.h>

#include <firestorm/engine/types/vector_t.h>
#include <firestorm/engine/memory/ChunkManager.h>
#include <firestorm/engine/worker/worker_t.h>
#include <firestorm/engine/vector_ops/dot_product_naive.h>
#if USE_AVX
#include <firestorm/engine/vector_ops/dot_product_avx256.h>
#endif
#include <firestorm/engine/vector_ops/dot_product_openmp.h>
#include <firestorm/engine/vector_ops/dot_product_sse42.h>
#include <firestorm/engine/mapper/mapper_factory.h>
#include <firestorm/benchmark/benchmark.h>
#include "query_vector.h"
#include "test_round.h"

using namespace std;

namespace firestorm {

    void run_benchmark(const shared_ptr<spdlog::logger> &log, const size_t num_vectors, const size_t target_chunk_size,
                       const boost::optional<size_t> num_workers) {
        const auto seed = 1337; // std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::normal_distribution<float> distribution(0.0f, 2.0f);
        auto random = std::bind(distribution, generator);

        log->info("Initializing vectors ...");
        auto expected = new float[num_vectors];
        auto result = new float[num_vectors];

        // We first create a chunk manager that will hold the vectors.
        shared_ptr<ChunkManager> chunkManager = make_shared<ChunkManager>();
        const size_t num_vectors_per_chunk = target_chunk_size / (BENCHMARK_NUM_DIMENSIONS * sizeof(float));

        // A worker is a visitor that is performs a calculation on the chunks of a
        // registered manager.
        unique_ptr<worker_t> worker_st = make_unique<worker_t>();

        // Add multiple workers for multi-threaded testing
        size_t concurrency = num_workers ? num_workers.get() : thread::hardware_concurrency();
        if (concurrency == 0) concurrency = 8;
        log->info("Use concurrency of {} threads.", concurrency);
        worker_thread_coordinator coordinator {concurrency};

        // To simplify experiments, we require the block to exactly match our expectations
        // about vector lengths. Put differently, all bytes in the buffer can be used.
        if ((target_chunk_size % (sizeof(float) * BENCHMARK_NUM_DIMENSIONS)) != 0) {
            log->error("Chunk size must be able to fully contain all vectors.");
            return;
        }

        shared_ptr<mem_chunk_t> chunk = nullptr;
        auto remaining_chunk_size = 0_B;    // number of remaining bytes in the current chunk
        size_t float_offset = 0;            // index into the current buffer, counts floats

        // TODO: Allocate separate chunk for vector norms
        // TODO: Sort out elements by NaN for unused norms, e.g. https://stackoverflow.com/questions/31818755/comparison-with-nan-using-avx

        // Keep track of the results for validation.
        score_t expected_best_match {};

        // Create a random query vector.
        auto query = create_query_vector(BENCHMARK_NUM_DIMENSIONS);

        // Create M vectors (1000, 10000, whatever).
        for (size_t j = 0; j < num_vectors; ++j) {

            // Initial condition, also reached during runtime:
            // If one memory chunk is "full", allocate another one.
            if (remaining_chunk_size == 0_B) {
                log->debug("Allocating chunk.");

                chunk = chunkManager->allocate(num_vectors_per_chunk, BENCHMARK_NUM_DIMENSIONS);
                assert(chunk != nullptr);

                remaining_chunk_size = target_chunk_size;
                float_offset = 0;

                // Assign the chunk
                worker_st->assign_chunk(chunk);
                coordinator.assign_chunk(chunk);
            }

            // Some progress printing.
            if (j % 2500 == 0) {
                log->info("- {}/{}", j, num_vectors);
            }

            auto a = &chunk->data[float_offset];
            const auto *const b = &query->data[0];

            assert(a != nullptr);
            assert(b != nullptr);
            assert(float_offset < num_vectors * BENCHMARK_NUM_DIMENSIONS);
            assert(remaining_chunk_size >= sizeof(float) * BENCHMARK_NUM_DIMENSIONS);

            remaining_chunk_size -= (sizeof(float) * BENCHMARK_NUM_DIMENSIONS);
            float_offset += BENCHMARK_NUM_DIMENSIONS;

            // Create a vector.
            for (size_t i = 0; i < BENCHMARK_NUM_DIMENSIONS; ++i) {
                a[i] = random();
            }
            vec_normalize_naive(a, chunk->dimensions);

            // Obtain the expected result.
            expected[j] = 0;
            for (size_t i = 0; i < BENCHMARK_NUM_DIMENSIONS; ++i) {
                expected[j] += a[i] * b[i];
            }

            if ((expected_best_match < expected[j]) || expected_best_match.invalid()) {
                expected_best_match = {{chunk->index, static_cast<vector_idx_t >(j)}, expected[j]};
            }
        }
        log->info("- {}/{}", num_vectors, num_vectors);
        log->info("Vectors initialized"); // TODO: Add timing

        const size_t repetitions = 10;

#if USE_AVX

        log->info("dot_product_avx256");
        run_test_round<dot_product_avx256_t>(log, repetitions, result, *chunkManager, query,
                                             expected_best_match, num_vectors);

        log->info("dot_product_avx256 (Worker)");
        run_test_round_worker<dot_product_avx256_t>(log, repetitions, *worker_st, query,
                                                    expected_best_match, num_vectors);

        log->info("dot_product_avx256 (MT workers)");
        run_test_round_worker<dot_product_avx256_t>(log, repetitions, coordinator, query,
                                                    expected_best_match, num_vectors);

#endif

#if USE_OPENMP

        log->info("dot_product_openmp");
        run_test_round<dot_product_openmp_t>(log, repetitions, result, *chunkManager, query,
                                             expected_best_match, num_vectors);

        log->info("dot_product_openmp (worker_t)");
        run_test_round_worker<dot_product_openmp_t>(log, repetitions, *worker_st, query,
                                                    expected_best_match, num_vectors);

        log->info("dot_product_openmp (MT workers)");
        run_test_round_worker<dot_product_openmp_t>(log, repetitions, coordinator, query,
                                                    expected_best_match, num_vectors);

#endif

#if USE_SSE == 4

        log->info("dot_product_sse42");
        run_test_round<dot_product_sse42_t>(log, repetitions, result, *chunkManager, query,
                                            expected_best_match, num_vectors);

        log->info("dot_product_sse42 (worker_t)");
        run_test_round_worker<dot_product_sse42_t>(log, repetitions, *worker_st, query,
                                                   expected_best_match, num_vectors);

        log->info("dot_product_sse42 (MT workers)");
        run_test_round_worker<dot_product_sse42_t>(log, repetitions, coordinator, query,
                                                   expected_best_match, num_vectors);

#endif

        log->info("dot_product_unrolled_8");
        run_test_round<dot_product_unrolled_8_t>(log, repetitions, result, *chunkManager, query,
                                                 expected_best_match, num_vectors);

        log->info("dot_product_unrolled_8 (worker_t)");
        run_test_round_worker<dot_product_unrolled_8_t>(log, repetitions, *worker_st, query,
                                                        expected_best_match, num_vectors);

        log->info("dot_product_unrolled_8 (MT workers)");
        run_test_round_worker<dot_product_unrolled_8_t>(log, repetitions, coordinator, query,
                                                        expected_best_match, num_vectors);

        log->info("dot_product_naive");
        run_test_round<dot_product_naive_t>(log, repetitions, result, *chunkManager, query,
                                            expected_best_match, num_vectors);

        log->info("dot_product_naive (worker_t)");
        run_test_round_worker<dot_product_naive_t>(log, repetitions, *worker_st, query,
                                                   expected_best_match, num_vectors);

        log->info("dot_product_naive (MT workers)");
        run_test_round_worker<dot_product_naive_t>(log, repetitions, coordinator, query,
                                                   expected_best_match, num_vectors);

        log->info("Cleaning up ...");
        delete[] expected;
        delete[] result;

        log->info("Done.");
    }

}
