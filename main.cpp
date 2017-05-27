#include <iostream>
#include <random>
#include <chrono>
#include <functional>
#include <memory>

#include "Simd.h"
#include "ChunkManager.h"
#include "Worker.h"

// TODO: Boost
// TODO: Boost.SIMD

float dot_product_avx256(const float *const a_row, const float *const b_row, const size_t N) {
    auto total = _mm256_set1_ps(0.0f);
    for (size_t i = 0; i < N; i += 32) {
        // Prefetch the next batch into L2 - saves around 40ms on 2 million 2048-float rows.
        _mm_prefetch(&a_row[i + 32 * 8], _MM_HINT_T1);
        _mm_prefetch(&b_row[i + 32 * 8], _MM_HINT_T1);

        // Load 32 floats per vector.
        const auto a0 = _mm256_load_ps(&a_row[i]);
        const auto b0 = _mm256_load_ps(&b_row[i]);
        const auto a1 = _mm256_load_ps(&a_row[i + 8]);
        const auto b1 = _mm256_load_ps(&b_row[i + 8]);
        const auto a2 = _mm256_load_ps(&a_row[i + 16]);
        const auto b2 = _mm256_load_ps(&b_row[i + 16]);
        const auto a3 = _mm256_load_ps(&a_row[i + 24]);
        const auto b3 = _mm256_load_ps(&b_row[i + 24]);

        // Do separate dot products.
        const auto c0 = _mm256_dp_ps(a0, b0, 0xff);
        const auto c1 = _mm256_dp_ps(a1, b1, 0xff);
        const auto c2 = _mm256_dp_ps(a2, b2, 0xff);
        const auto c3 = _mm256_dp_ps(a3, b3, 0xff);

        // Do separate partial sums.
        const auto p0 = _mm256_add_ps(c0, c1);
        const auto p1 = _mm256_add_ps(c2, c3);

        // Aggregate the partial sums and allocate to running total.
        const auto s = _mm256_add_ps(p0, p1);
        total = _mm256_add_ps(total, s);
    }

    alignas(32) float ptr[8];
    _mm256_store_ps(ptr, total);
    return ptr[0] + ptr[5];
}

float dot_product_naive(const float* const a_row, const float* const b_row, const size_t N) {
    auto total = 0.0f;
    for (size_t i = 0; i < N; ++i) {
        total += a_row[i] * b_row[i];
    }
    return total;
}

float dot_product_unrolled_8(const float *const a_row, const float *const b_row, const size_t N) {
    auto total = 0.0f;
    for (size_t i = 0; i < N; i += 8) {
        total += a_row[i] * b_row[i] +
                a_row[i + 1] * b_row[i + 1] +
                a_row[i + 2] * b_row[i + 2] +
                a_row[i + 3] * b_row[i + 3] +
                a_row[i + 4] * b_row[i + 4] +
                a_row[i + 5] * b_row[i + 5] +
                a_row[i + 6] * b_row[i + 6] +
                a_row[i + 7] * b_row[i + 7];
    }
    return total;
}

int what() {
    Worker worker;

    const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(0.0f, 2.0f);
    auto random = std::bind(distribution, generator);

    const size_t N = 2048;
    const size_t M = 100000;

    auto expected = new float[M];
    auto result = new float[M];

    std::cout << "Initializing vectors ..." << std::endl;
    ChunkManager chunkManager_a;
    ChunkManager chunkManager_b;
    constexpr const auto chunk_size = 32_MB;

    // To simplify experiments, we require the block to exactly match our expectations
    // about vector lengths. Put differently, all bytes in the buffer can be used.
    static_assert((chunk_size % (sizeof(float)*N)) == 0);

    std::shared_ptr<mem_chunk_t> chunk_a = nullptr;
    std::shared_ptr<mem_chunk_t> chunk_b = nullptr;
    auto remaining_chunk_size = 0_B;    // number of remaining bytes in the current chunk
    size_t float_offset = 0;            // index into the current buffer, counts floats

    // Keep track of the total sum for validation.
    auto expected_total_sum = 0.0f;

    for (size_t j = 0; j < M; ++j) {

        // Initial condition, also reached during runtime: If one block is full,
        // allocate another one.
        if (remaining_chunk_size == 0_B) {
            std::cout << "Allocating chunk." << std::endl;

            chunk_a = chunkManager_a.allocate(chunk_size).lock();
            chunk_b = chunkManager_b.allocate(chunk_size).lock();
            remaining_chunk_size = chunk_size;
            float_offset = 0;
        }

        if (j % 2500 == 0) {
            std::cout << "- " << j << "/" << M << std::endl;
        }

        expected[j] = 0;
        auto a = &chunk_a->data[float_offset];
        auto b = &chunk_b->data[float_offset];

        assert(float_offset < M*N);
        assert(remaining_chunk_size >= sizeof(float)*N);

        remaining_chunk_size -= (sizeof(float)*N);
        float_offset += N;

        // Write one vector and one expected result.
        for (size_t i = 0; i < N; ++i) {
            a[i] = random();
            b[i] = random();
            expected[j] += a[i] * b[i];
            expected_total_sum += a[i] * b[i];
        }
    }
    std::cout << "- " << M << "/" << M << std::endl
              << "Vectors initialized." << std::endl;

    const size_t repetitions = 20;
    for (size_t repetition = 0; repetition < repetitions; ++repetition)
    {
        std::cout << "Running test round " << (repetition+1) << " of " << repetitions << " ..." << std::endl;

        // Keep track of the total sum for validation.
        auto total_sum = 0.0f;

        chunk_idx_t current_chunk = 0;
        chunk_a = chunkManager_a.get(current_chunk).lock();
        chunk_b = chunkManager_b.get(current_chunk).lock();
        float_offset = 0;

        const auto vectors_per_chunk = chunk_size / (N * sizeof(float));
        auto remaining_vectors_per_chunk = vectors_per_chunk;

        auto start_time = std::chrono::high_resolution_clock::now();

        // Run for a couple of test iterations ...
        for (size_t vector_idx = 0; vector_idx < M; ++vector_idx) {

            if (remaining_vectors_per_chunk == 0) {
                current_chunk += 1;
                chunk_a = chunkManager_a.get(current_chunk).lock();
                chunk_b = chunkManager_b.get(current_chunk).lock();

                float_offset = 0;
                remaining_vectors_per_chunk = vectors_per_chunk;
            }

            auto a_row = &chunk_a->data[float_offset];
            auto b_row = &chunk_b->data[float_offset];

            --remaining_vectors_per_chunk;
            float_offset += N;

            // Calculate the dot product of the 2048-element vector
            static_assert((N & 31) == 0, "Vector length must be a multiple of 32 elements.");
            const auto dot_product = dot_product_avx256(a_row, b_row, N);
            // const auto dot_product = dot_product_naive(a_row, b_row, N);
            // const auto dot_product = dot_product_unrolled_8(a_row, b_row, N);

            result[vector_idx] = dot_product;
            total_sum += dot_product;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        auto vectors_per_second = static_cast<float>(M * 1000) / static_cast<float>(duration);
        std::cout << "Sum: " << total_sum << " (expected: " << expected_total_sum << ")" << " - duration: " << duration << "ms"
                  << " (" << vectors_per_second << " vector/s)"
                  << std::endl;
    }

    std::cout << "Cleaning up ..." << std::endl;
    delete expected;
    delete result;

    std::cout << "Done." << std::endl;
}

int main() {
    if (avx2_enabled()) {
        std::cout << "AVX2 available!" << std::endl;
    }

    if (avx_enabled()) {
        std::cout << "AVX available!" << std::endl;
    }

    if (!avx2_enabled() && !avx_enabled()) {
        std::cout << "AVX/AVX2 support is required." << std::endl;
        return 1;
    }

    what();

    std::cin.get();
    return 0;
}