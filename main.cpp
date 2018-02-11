#include <iostream>
#include <random>
#include <chrono>
#include <functional>
#include <memory>

#include <gperftools/profiler.h>

#include "Simd.h"
#include "ChunkManager.h"
#include "Worker.h"
#include "DotProductVisitor.h"
#include "dot_product_naive.h"

// TODO: Boost
// TODO: Boost.SIMD
// TODO: OpenMP backed loops
// TODO: determine __restrict__ keyword support from https://github.com/elemental/Elemental/blob/master/cmake/detect/CXX.cmake

const size_t N = 2048;
const size_t M = 2000;

vector_t create_query_vector() {
    const auto seed = 0L;
    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(0.0f, 2.0f);
    auto random = std::bind(distribution, generator);

    // Create a simple query vector
    vector_t query {N};
    for (size_t i = 0; i < N; ++i) {
        query.data[i] = random();
    }

#if AVX2 || AVX
    auto norm = vec_normalize_avx256(query.data, query.dimensions);
    auto norm2 = vec_norm_avx256(query.data, query.dimensions);
#else
    auto norm = vec_normalize_naive(query.data, query.dimensions);
    auto norm2 = vec_norm_naive(query.data, query.dimensions);
#endif

    std::cout << "Test vector norm before normalizing is " << norm << " (" << norm2 << " after that)." << std::endl;
    return query;
}

template <typename T>
void run_test_round(float *const result, const ChunkManager &chunkManager,
                    const vector_t& query, const bytes_t chunk_size, float expected_total_sum) {

    static_assert(std::is_convertible<T*, dot_product_t*>::value, "Derived type must inherit dot_product_t as public");
    const T calculate {};

    // Keep track of the total sum for validation.
    auto best_match = 0.0f;
    auto best_match_idx = static_cast<size_t>(-1);

    chunk_idx_t current_chunk = 0;
    auto chunk = chunkManager.get_ro(current_chunk);
    auto query_vector = query.data;

    auto float_offset = 0;

    const auto vectors_per_chunk = chunk_size / (N * sizeof(float));
    auto remaining_vectors_per_chunk = vectors_per_chunk;

    auto start_time = std::chrono::_V2::system_clock::now();

    // Run over all vectors ...
    for (size_t vector_idx = 0; vector_idx < M; ++vector_idx) {

        if (remaining_vectors_per_chunk == 0) {
            current_chunk += 1;
            chunk = chunkManager.get_ro(current_chunk);

            float_offset = 0;
            remaining_vectors_per_chunk = vectors_per_chunk;
        }

        auto ref_vector = &chunk->data[float_offset];

        --remaining_vectors_per_chunk;
        float_offset += N;

        // Calculate the dot product of the 2048-element vector
        static_assert((N & 31) == 0, "Vector length must be a multiple of 32 elements.");
        const auto dot_product = calculate(ref_vector, query_vector, N);

        result[vector_idx] = dot_product;

        if (dot_product > best_match) {
            best_match = dot_product;
            best_match_idx = vector_idx;
        }
    }

    auto end_time = std::chrono::_V2::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    auto vectors_per_second = static_cast<float>(M * 1000) / static_cast<float>(duration);
    std::cout << "Best match: " << best_match << " at " << best_match_idx << " (expected: " << expected_total_sum << ")"
              << " - duration: " << duration << "ms"
              << " (" << vectors_per_second << " vector/s)"
              << std::endl;
}

template <typename T>
void run_test_round_worker(const Worker &worker,
                    const vector_t& query, float expected_total_sum) {

    const DotProductVisitor<T> visitor {};
    auto results = worker.create_result_buffer();

    // Keep track of the total sum for validation.
    auto best_match = 0.0f;
    auto best_match_idx = static_cast<size_t>(-1);

    auto start_time = std::chrono::_V2::system_clock::now();
    worker.accept(visitor, query, results);
    auto end_time = std::chrono::_V2::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    // TODO: This should eventually be part of the worker, otherwise we're going through the lists twice.
    for (size_t chunk_idx = 0; chunk_idx < results.size(); ++chunk_idx ) {
        // TODO: make use of the chunk index.
        auto chunk_result = results.at(chunk_idx);
        for (size_t vector_idx = 0; vector_idx < M; ++vector_idx) {
            const auto score = chunk_result->scores[vector_idx];
            if (score > best_match) {
                best_match_idx = static_cast<size_t>(vector_idx);
                best_match = score;
            }
        }
    }

    auto vectors_per_second = static_cast<float>(M * 1000) / static_cast<float>(duration);
    std::cout << "Best match: " << best_match << " at " << best_match_idx << " (expected: " << expected_total_sum << ")"
              << " - duration: " << duration << "ms"
              << " (" << vectors_per_second << " vector/s)"
              << std::endl;
}

void what() {

    const auto seed = 1337; // std::chrono::system_clock::now().time_since_epoch().count();

    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(0.0f, 2.0f);
    auto random = std::bind(distribution, generator);

    auto expected = new float[M];
    auto result = new float[M];

    // We first create two chunk managers that will hold the vectors.
    std::cout << "Initializing vectors ..." << std::endl;
    std::shared_ptr<ChunkManager> chunkManager = std::make_shared<ChunkManager>();
    constexpr const auto target_chunk_size = 32_MB;
    constexpr size_t num_vectors = target_chunk_size / (N*sizeof(float));

    // A worker is a visitor that is performs a calculation on the chunks of a
    // registered manager.
    std::unique_ptr<Worker> worker = std::make_unique<Worker>(chunkManager);

    // To simplify experiments, we require the block to exactly match our expectations
    // about vector lengths. Put differently, all bytes in the buffer can be used.
    static_assert((target_chunk_size % (sizeof(float)*N)) == 0);

    std::shared_ptr<mem_chunk_t> chunk = nullptr;
    auto remaining_chunk_size = 0_B;    // number of remaining bytes in the current chunk
    size_t float_offset = 0;            // index into the current buffer, counts floats

    // TODO: Allocate separate chunk for vector norms
    // TODO: Sort out elements by NaN for unused norms, e.g. https://stackoverflow.com/questions/31818755/comparison-with-nan-using-avx

    // Keep track of the results for validation.
    auto expected_best_match = 0.0f;
    auto expected_best_match_idx = static_cast<size_t>(-1);

    // Create a random query vector.
    vector_t query = create_query_vector();

    // Create M vectors (1000, 10000, whatever).
    for (size_t j = 0; j < M; ++j) {

        // Initial condition, also reached during runtime:
        // If one memory chunk is "full", allocate another one.
        if (remaining_chunk_size == 0_B) {
            std::cout << "Allocating chunk." << std::endl;

            chunk = chunkManager->allocate(num_vectors, N);
            assert(chunk != nullptr);

            remaining_chunk_size = target_chunk_size;
            float_offset = 0;

            worker->assign_chunk(chunk->index);
        }

        // Some progress printing.
        if (j % 2500 == 0) {
            std::cout << "- " << j << "/" << M << std::endl;
        }

        auto a = &chunk->data[float_offset];
        const auto *const b = &query.data[0];

        assert(float_offset < M*N);
        assert(remaining_chunk_size >= sizeof(float)*N);

        remaining_chunk_size -= (sizeof(float)*N);
        float_offset += N;

        // Create a vector.
        for (size_t i = 0; i < N; ++i) {
            a[i] = random();
        }
        vec_normalize_naive(a, chunk->dimensions);

        // Obtain the expected result.
        expected[j] = 0;
        for (size_t i = 0; i < N; ++i) {
            expected[j] += a[i] * b[i];
        }

        if (expected[j] > expected_best_match) {
            expected_best_match = expected[j];
            expected_best_match_idx = j;
        }
    }
    std::cout << "- " << M << "/" << M << std::endl
              << "Vectors initialized." << std::endl;

    // Worker test
#if AVX2 || AVX
    DotProductVisitor<dot_product_avx256_t> visitor;
#else
    DotProductVisitor<dot_product_unrolled_8_t> visitor;
#endif

    const size_t repetitions = 20;

#if AVX_VERSION

    std::cout << std::endl;
    std::cout << "dot_product_avx256" << std::endl
              << "------------------" << std::endl;
    for (size_t repetition = 0; repetition < repetitions; ++repetition)
    {
        std::cout << "test round " << (repetition+1) << " of " << repetitions << " ... ";
        run_test_round<dot_product_avx256_t>(result, *chunkManager, query, chunk_size, expected_best_match_idx);
    }

    std::cout << std::endl;
    std::cout << "dot_product_avx256 (Worker)" << std::endl
              << "---------------------------" << std::endl;
    for (size_t repetition = 0; repetition < repetitions; ++repetition)
    {
        std::cout << "test round " << (repetition+1) << " of " << repetitions << " ... ";
        run_test_round_worker<dot_product_avx256_t>(*worker, query, expected_best_match_idx);
    }

#endif

    std::cout << std::endl;
    std::cout << "dot_product_unrolled_8" << std::endl
              << "----------------------" << std::endl;
    for (size_t repetition = 0; repetition < repetitions; ++repetition)
    {
        std::cout << "test round " << (repetition+1) << " of " << repetitions << " ... ";
        run_test_round<dot_product_unrolled_8_t>(result, *chunkManager, query, target_chunk_size, expected_best_match_idx);
    }

    std::cout << std::endl;
    std::cout << "dot_product_unrolled_8 (Worker)" << std::endl
              << "-------------------------------" << std::endl;
    for (size_t repetition = 0; repetition < repetitions; ++repetition)
    {
        std::cout << "test round " << (repetition+1) << " of " << repetitions << " ... ";
        run_test_round_worker<dot_product_unrolled_8_t>(*worker, query, expected_best_match_idx);
    }

    std::cout << std::endl;
    std::cout << "dot_product_naive" << std::endl
              << "-----------------" << std::endl;
    for (size_t repetition = 0; repetition < repetitions; ++repetition)
    {
        std::cout << "test round " << (repetition+1) << " of " << repetitions << " ... ";
        run_test_round<dot_product_naive_t>(result, *chunkManager, query, target_chunk_size, expected_best_match_idx);
    }

    std::cout << std::endl;
    std::cout << "dot_product_naive (Worker)" << std::endl
              << "--------------------------" << std::endl;
    for (size_t repetition = 0; repetition < repetitions; ++repetition)
    {
        std::cout << "test round " << (repetition+1) << " of " << repetitions << " ... ";
        run_test_round_worker<dot_product_naive_t>(*worker, query, expected_best_match_idx);
    }

    std::cout << "Cleaning up ..." << std::endl;
    delete[] expected;
    delete[] result;

    std::cout << "Done." << std::endl;
}

int main() {

#if WITH_GPERFTOOLS
    ProfilerState state {};
    ProfilerGetCurrentState(&state);
    std::cout << "Profiling enabled: " << (state.enabled ? "yes" : "no") << std::endl;
#endif

    if (avx2_enabled()) {
        std::cout << "AVX2 available!" << std::endl;
    }

    if (avx_enabled()) {
        std::cout << "AVX available!" << std::endl;
    }

    if (!avx2_enabled() && !avx_enabled()) {
        std::cout << "AVX/AVX2 support is required for optimal performance." << std::endl;
    }

    what();

    return 0;
}
