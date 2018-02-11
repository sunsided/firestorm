#include <iostream>
#include <random>
#include <chrono>
#include <functional>
#include <memory>

#include <Eigen/CXX11/Tensor>
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
const size_t M = 10000;

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

float dot_product_eigen(float * const a_row, float * const b_row, const size_t _) {
    // the _ parameter is assumed to be exactly N
    auto ma = Eigen::Map<Eigen::Matrix<float, 1, N>, Eigen::Aligned32>(a_row);
    auto mb = Eigen::Map<Eigen::Matrix<float, N, 1>, Eigen::Aligned32>(b_row);
    return ma.dot(mb);
}

const Eigen::array<Eigen::IndexPair<int>, Eigen::RowMajor> contraction_pair00 { Eigen::IndexPair<int>(0, 0) };

float dot_product_eigen_tensor(const float * const a_row, const float * const b_row, const size_t _) {
    // the _ parameter is assumed to be exactly N
    auto ma = Eigen::TensorMap<Eigen::TensorFixedSize<const float, Eigen::Sizes<N>, Eigen::RowMajor, int>, Eigen::Aligned32>(a_row, N);
    auto mb = Eigen::TensorMap<Eigen::TensorFixedSize<const float, Eigen::Sizes<N>, Eigen::RowMajor, int>, Eigen::Aligned32>(b_row, N);

    const auto op = ma.contract(mb, contraction_pair00); // ma.dot(mb);
    const Eigen::TensorFixedSize<float, Eigen::Sizes<>, Eigen::RowMajor, int> result = op;
    return result(0);
}

template <typename DotProductFunc>
void run_test_round(float *const result, const ChunkManager &chunkManager_a,
                    const ChunkManager &chunkManager_b, const bytes_t chunk_size, float expected_total_sum,
                    DotProductFunc && calculate) {

    // Keep track of the total sum for validation.
    auto total_sum = 0.0f;

    chunk_idx_t current_chunk = 0;
    auto chunk_a = chunkManager_a.get_ro(current_chunk).lock();
    auto chunk_b = chunkManager_b.get_ro(current_chunk).lock();
    auto float_offset = 0;

    const auto vectors_per_chunk = chunk_size / (N * sizeof(float));
    auto remaining_vectors_per_chunk = vectors_per_chunk;

    auto start_time = std::chrono::_V2::system_clock::now();

    // Run for a couple of test iterations ...
    for (size_t vector_idx = 0; vector_idx < M; ++vector_idx) {

        if (remaining_vectors_per_chunk == 0) {
            current_chunk += 1;
            chunk_a = chunkManager_a.get_rw(current_chunk).lock();
            chunk_b = chunkManager_b.get_rw(current_chunk).lock();

            float_offset = 0;
            remaining_vectors_per_chunk = vectors_per_chunk;
        }

        auto a_row = &chunk_a->data[float_offset];
        auto b_row = &chunk_b->data[float_offset];

        --remaining_vectors_per_chunk;
        float_offset += N;

        // Calculate the dot product of the 2048-element vector
        static_assert((N & 31) == 0, "Vector length must be a multiple of 32 elements.");
        const auto dot_product = calculate(a_row, b_row, N);

        result[vector_idx] = dot_product;
        total_sum += dot_product;
    }

    auto end_time = std::chrono::_V2::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    auto vectors_per_second = static_cast<float>(M * 1000) / static_cast<float>(duration);
    std::cout << "Sum: " << total_sum << " (expected: " << expected_total_sum << ")"
              << " - duration: " << duration << "ms"
              << " (" << vectors_per_second << " vector/s)"
              << std::endl;
}

int what() {

    const auto seed = 1337; // std::chrono::system_clock::now().time_since_epoch().count();

    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(0.0f, 2.0f);
    auto random = std::bind(distribution, generator);

    auto expected = new float[M];
    auto result = new float[M];

    // We first create two chunk managers that will hold the vectors.
    std::cout << "Initializing vectors ..." << std::endl;
    std::shared_ptr<ChunkManager> chunkManager_a = std::make_shared<ChunkManager>();
    constexpr const auto target_chunk_size = 32_MB;
    constexpr size_t num_vectors = target_chunk_size / (N*sizeof(float));

    // A worker is a visitor that is performs a calculation on the chunks of a
    // registered manager.
    std::unique_ptr<Worker> worker = std::make_unique<Worker>(chunkManager_a);

    // To simplify experiments, we require the block to exactly match our expectations
    // about vector lengths. Put differently, all bytes in the buffer can be used.
    static_assert((target_chunk_size % (sizeof(float)*N)) == 0);

    std::shared_ptr<mem_chunk_t> chunk_a = nullptr;
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

            chunk_a = chunkManager_a->allocate(num_vectors, N);
            assert(chunk_a != nullptr);

            remaining_chunk_size = target_chunk_size;
            float_offset = 0;

            worker->assign_chunk(chunk_a->index);
        }

        // Some progress printing.
        if (j % 2500 == 0) {
            std::cout << "- " << j << "/" << M << std::endl;
        }

        auto a = &chunk_a->data[float_offset];
        const auto *const b = &query.data[0];

        assert(float_offset < M*N);
        assert(remaining_chunk_size >= sizeof(float)*N);

        remaining_chunk_size -= (sizeof(float)*N);
        float_offset += N;

        // Create a vector.
        for (size_t i = 0; i < N; ++i) {
            a[i] = random();
        }
        vec_normalize_naive(a, chunk_a->dimensions);

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
        std::cout << "round " << (repetition+1) << " of " << repetitions << " ..." << std::endl;
        run_test_round(result, chunkManager_a, chunkManager_b, chunk_size, expected_total_sum, dot_product_avx256);
    }

#endif

    std::cout << std::endl;
    std::cout << "dot_product_eigen" << std::endl
              << "-----------------" << std::endl;
    for (size_t repetition = 0; repetition < repetitions; ++repetition)
    {
        std::cout << "test round " << (repetition+1) << " of " << repetitions << " ..." << std::endl;
        run_test_round(result, chunkManager_a, chunkManager_b, chunk_size, expected_total_sum, dot_product_eigen);
    }

    std::cout << std::endl;
    std::cout << "dot_product_eigen_tensor (SYCL)" << std::endl
              << "-------------------------------" << std::endl;
    for (size_t repetition = 0; repetition < repetitions; ++repetition)
    {
        std::cout << "test round " << (repetition+1) << " of " << repetitions << " ..." << std::endl;
        run_test_round(result, chunkManager_a, chunkManager_b, chunk_size, expected_total_sum, dot_product_eigen_tensor);
    }

    std::cout << std::endl;
    std::cout << "dot_product_unrolled_8" << std::endl
              << "----------------------" << std::endl;
    for (size_t repetition = 0; repetition < repetitions; ++repetition)
    {
        std::cout << "test round " << (repetition+1) << " of " << repetitions << " ..." << std::endl;
        run_test_round(result, chunkManager_a, chunkManager_b, chunk_size, expected_total_sum, dot_product_unrolled_8);
    }

    std::cout << std::endl;
    std::cout << "dot_product_naive" << std::endl
              << "-----------------" << std::endl;
    for (size_t repetition = 0; repetition < repetitions; ++repetition)
    {
        std::cout << "test round " << (repetition+1) << " of " << repetitions << " ..." << std::endl;
        run_test_round(result, chunkManager_a, chunkManager_b, chunk_size, expected_total_sum, dot_product_naive);
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