//
// Created by sunside on 24.02.18.
//

#include <memory>
#include <random>
#include <spdlog/spdlog.h>
#include <firestorm/vector_t.h>
#include <firestorm/ChunkManager.h>
#include <firestorm/Worker.h>
#include <firestorm/dot_product_naive.h>
#if USE_AVX
#include <firestorm/dot_product_avx256.h>
#endif
#include <firestorm/dot_product_openmp.h>
#include <firestorm/dot_product_sse42.h>
#include "benchmark.h"

using namespace std;

const auto MS_TO_S = 1000.0F;
const size_t NUM_DIMENSIONS = 2048;

/// Builds a query vector to test against.
/// \param NUM_DIMENSIONS The dimensionality of the test vector.
/// \return
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

void run_benchmark(const shared_ptr<spdlog::logger> &log, const size_t num_vectors) {
    const auto seed = 1337; // std::chrono::system_clock::now().time_since_epoch().count();

    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(0.0f, 2.0f);
    auto random = std::bind(distribution, generator);

    log->info("Initializing vectors ...");
    auto expected = new float[num_vectors];
    auto result = new float[num_vectors];

    // We first create a chunk manager that will hold the vectors.
    std::shared_ptr<ChunkManager> chunkManager = std::make_shared<ChunkManager>();
    constexpr const auto target_chunk_size = 32_MB;
    constexpr size_t num_vectors_per_chunk = target_chunk_size / (NUM_DIMENSIONS*sizeof(float));

    // A worker is a visitor that is performs a calculation on the chunks of a
    // registered manager.
    std::unique_ptr<Worker> worker = std::make_unique<Worker>(chunkManager);

    // To simplify experiments, we require the block to exactly match our expectations
    // about vector lengths. Put differently, all bytes in the buffer can be used.
    static_assert((target_chunk_size % (sizeof(float)*NUM_DIMENSIONS)) == 0, "Chunk size must be able to fully contain all vectors.");

    std::shared_ptr<mem_chunk_t> chunk = nullptr;
    auto remaining_chunk_size = 0_B;    // number of remaining bytes in the current chunk
    size_t float_offset = 0;            // index into the current buffer, counts floats

    // TODO: Allocate separate chunk for vector norms
    // TODO: Sort out elements by NaN for unused norms, e.g. https://stackoverflow.com/questions/31818755/comparison-with-nan-using-avx

    // Keep track of the results for validation.
    auto expected_best_match = 0.0f;
    auto expected_best_match_idx = static_cast<size_t>(-1);

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

            worker->assign_chunk(chunk->index);
        }

        // Some progress printing.
        if (j % 2500 == 0) {
            log->info("- {}/{}", j, num_vectors);
        }

        auto a = &chunk->data[float_offset];
        const auto *const b = &query.data[0];

        assert(float_offset < num_vectors*NUM_DIMENSIONS);
        assert(remaining_chunk_size >= sizeof(float)*NUM_DIMENSIONS);

        remaining_chunk_size -= (sizeof(float)*NUM_DIMENSIONS);
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
            expected_best_match = expected[j];
            expected_best_match_idx = j;
        }
    }
    log->info("- {}/{}", num_vectors, num_vectors);
    log->info("Vectors initialized"); // TODO: Add timing

    const size_t repetitions = 20;

#if USE_AVX

    log->info("dot_product_avx256");
    run_test_round<dot_product_avx256_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                         expected_best_match_idx, expected_best_match, num_vectors);

    log->info("dot_product_avx256 (Worker)");
    run_test_round_worker<dot_product_avx256_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match, num_vectors);

#endif

#if USE_OPENMP

    log->info("dot_product_openmp");
    run_test_round<dot_product_openmp_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                         expected_best_match_idx, expected_best_match, num_vectors);

    log->info("dot_product_openmp (Worker)");
    run_test_round_worker<dot_product_openmp_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match, num_vectors);

#endif

#if USE_SSE == 4

    log->info("dot_product_sse42");
    run_test_round<dot_product_sse42_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                        expected_best_match_idx, expected_best_match, num_vectors);

    log->info("dot_product_sse42 (Worker)");
    run_test_round_worker<dot_product_sse42_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match, num_vectors);

#endif

    log->info("dot_product_unrolled_8");
    run_test_round<dot_product_unrolled_8_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                             expected_best_match_idx, expected_best_match, num_vectors);

    log->info("dot_product_unrolled_8 (Worker)");
    run_test_round_worker<dot_product_unrolled_8_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match, num_vectors);

    log->info("dot_product_naive");
    run_test_round<dot_product_naive_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                        expected_best_match_idx, expected_best_match, num_vectors);

    log->info("dot_product_naive (Worker)");
    run_test_round_worker<dot_product_naive_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match, num_vectors);

    log->info("Cleaning up ...");
    delete[] expected;
    delete[] result;

    log->info("Done.");
}