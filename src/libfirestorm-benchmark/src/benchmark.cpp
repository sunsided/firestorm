//
// Created by sunside on 24.02.18.
//

#include <thread>
#include <memory>
#include <random>
#include <algorithm>

#include <spdlog/spdlog.h>

#include <firestorm/engine/thread_support.h>
#include <firestorm/engine/types/vector_t.h>
#include <firestorm/engine/memory/ChunkManager.h>
#include <firestorm/engine/Worker.h>
#include <firestorm/engine/ops/dot_product_naive.h>
#if USE_AVX
#include <firestorm/engine/dot_product_avx256.h>
#endif
#include <firestorm/engine/ops/dot_product_openmp.h>
#include <firestorm/engine/ops/dot_product_sse42.h>
#include <firestorm/engine/mapper/mapper_factory.h>
#include <firestorm/engine/mapper/dot_product_mapper_factory.h>
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
        const size_t num_vectors_per_chunk = target_chunk_size / (NUM_DIMENSIONS * sizeof(float));

        // A worker is a visitor that is performs a calculation on the chunks of a
        // registered manager.
        unique_ptr<Worker> worker_st = make_unique<Worker>();

        // Add multiple workers for multi-threaded testing
        size_t concurrency = num_workers ? num_workers.get() : thread::hardware_concurrency();
        if (concurrency == 0) concurrency = 8;
        log->info("Use concurrency of {} threads.", concurrency);
        vector<unique_ptr<Worker>> workers_mt;
        for (size_t t = 0; t < concurrency; ++t) {
            workers_mt.push_back(make_unique<Worker>());
        }

        // To simplify experiments, we require the block to exactly match our expectations
        // about vector lengths. Put differently, all bytes in the buffer can be used.
        if ((target_chunk_size % (sizeof(float) * NUM_DIMENSIONS)) != 0) {
            log->error("Chunk size must be able to fully contain all vectors.");
            return;
        }

        shared_ptr<mem_chunk_t> chunk = nullptr;
        auto remaining_chunk_size = 0_B;    // number of remaining bytes in the current chunk
        size_t float_offset = 0;            // index into the current buffer, counts floats

        // TODO: Allocate separate chunk for vector norms
        // TODO: Sort out elements by NaN for unused norms, e.g. https://stackoverflow.com/questions/31818755/comparison-with-nan-using-avx

        // Keep track of the results for validation.
        auto expected_best_match = 0.0f;
        index_t expected_best_match_idx {0, 0};

        // Create a random query vector.
        vector_t query = create_query_vector(NUM_DIMENSIONS);

        // Create M vectors (1000, 10000, whatever).
        for (size_t j = 0; j < num_vectors; ++j) {

            // Initial condition, also reached during runtime:
            // If one memory chunk is "full", allocate another one.
            if (remaining_chunk_size == 0_B) {
                log->debug("Allocating chunk.");

                chunk = chunkManager->allocate(num_vectors_per_chunk, NUM_DIMENSIONS);
                assert(chunk != nullptr);

                remaining_chunk_size = target_chunk_size;
                float_offset = 0;

                worker_st->assign_chunk(chunk);

                // Round-robin assign the chunks.
                // TODO: Note that chunks should be shifted "from the right" to maximize likelihood of consecutive memory locations.
                auto chunk_to_add = chunk->index;
                auto worker_idx = chunk_to_add % workers_mt.size();
                workers_mt.at(worker_idx)->assign_chunk(chunk);
            }

            // Some progress printing.
            if (j % 2500 == 0) {
                log->info("- {}/{}", j, num_vectors);
            }

            auto a = &chunk->data[float_offset];
            const auto *const b = &query.data[0];

            assert(a != nullptr);
            assert(b != nullptr);
            assert(float_offset < num_vectors * NUM_DIMENSIONS);
            assert(remaining_chunk_size >= sizeof(float) * NUM_DIMENSIONS);

            remaining_chunk_size -= (sizeof(float) * NUM_DIMENSIONS);
            float_offset += NUM_DIMENSIONS;

            // Create a vector.
            for (size_t i = 0; i < NUM_DIMENSIONS; ++i) {
                a[i] = random();
            }
            vec_normalize_naive(a, chunk->dimensions);

            // Obtain the expected result.
            expected[j] = 0;
            for (size_t i = 0; i < NUM_DIMENSIONS; ++i) {
                expected[j] += a[i] * b[i];
            }

            if (expected[j] > expected_best_match) {
                // TODO: Make this a score_t
                expected_best_match = expected[j];
                expected_best_match_idx = {chunk->index, static_cast<vector_idx_t >(j)};
            }
        }
        log->info("- {}/{}", num_vectors, num_vectors);
        log->info("Vectors initialized"); // TODO: Add timing

        const size_t repetitions = 10;

#if USE_AVX

        log->info("dot_product_avx256");
        run_test_round<dot_product_avx256_t>(log, repetitions, result, *chunkManager, query,
                                             expected_best_match_idx, expected_best_match, num_vectors);

        log->info("dot_product_avx256 (Worker)");
        run_test_round_worker<dot_product_avx256_t>(log, repetitions, *worker_st, query,
                                                    expected_best_match_idx, expected_best_match, num_vectors);

        log->info("dot_product_avx256 (MT workers)");
        run_test_round_worker<dot_product_avx256_t>(log, repetitions, workers_mt, query,
                                                    expected_best_match_idx, expected_best_match, num_vectors);

#endif

#if USE_OPENMP

        log->info("dot_product_openmp");
        run_test_round<dot_product_openmp_t>(log, repetitions, result, *chunkManager, query,
                                             expected_best_match_idx, expected_best_match, num_vectors);

        log->info("dot_product_openmp (Worker)");
        run_test_round_worker<dot_product_openmp_t>(log, repetitions, *worker_st, query,
                                                    expected_best_match_idx, expected_best_match, num_vectors);

        log->info("dot_product_openmp (MT workers)");
        run_test_round_worker<dot_product_openmp_t>(log, repetitions, workers_mt, query,
                                                    expected_best_match_idx, expected_best_match, num_vectors);

#endif

#if USE_SSE == 4

        log->info("dot_product_sse42");
        run_test_round<dot_product_sse42_t>(log, repetitions, result, *chunkManager, query,
                                            expected_best_match_idx, expected_best_match, num_vectors);

        log->info("dot_product_sse42 (Worker)");
        run_test_round_worker<dot_product_sse42_t>(log, repetitions, *worker_st, query,
                                                   expected_best_match_idx, expected_best_match, num_vectors);

        log->info("dot_product_sse42 (MT workers)");
        run_test_round_worker<dot_product_sse42_t>(log, repetitions, workers_mt, query,
                                                   expected_best_match_idx, expected_best_match, num_vectors);

#endif

        log->info("dot_product_unrolled_8");
        run_test_round<dot_product_unrolled_8_t>(log, repetitions, result, *chunkManager, query,
                                                 expected_best_match_idx, expected_best_match, num_vectors);

        log->info("dot_product_unrolled_8 (Worker)");
        run_test_round_worker<dot_product_unrolled_8_t>(log, repetitions, *worker_st, query,
                                                        expected_best_match_idx, expected_best_match, num_vectors);

        log->info("dot_product_unrolled_8 (MT workers)");
        run_test_round_worker<dot_product_unrolled_8_t>(log, repetitions, workers_mt, query,
                                                        expected_best_match_idx, expected_best_match, num_vectors);

        log->info("dot_product_naive");
        run_test_round<dot_product_naive_t>(log, repetitions, result, *chunkManager, query,
                                            expected_best_match_idx, expected_best_match, num_vectors);

        log->info("dot_product_naive (Worker)");
        run_test_round_worker<dot_product_naive_t>(log, repetitions, *worker_st, query,
                                                   expected_best_match_idx, expected_best_match, num_vectors);

        log->info("dot_product_naive (MT workers)");
        run_test_round_worker<dot_product_naive_t>(log, repetitions, workers_mt, query, expected_best_match_idx,
                                                   expected_best_match, num_vectors);

        log->info("Cleaning up ...");
        delete[] expected;
        delete[] result;

        log->info("Done.");
    }

}
