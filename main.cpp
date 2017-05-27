#include <iostream>
#include <random>
#include <chrono>
#include <functional>
#include <vector>
#include <memory>
#include <boost/align/aligned_alloc.hpp>
#include "Simd.h"

// TODO: Boost
// TODO: Boost.SIMD


// TODO: Number of elements needs to be identical for all vectors, so is identical in whole table.
// TODO: Indexing can be performed on a separate instance.

struct alignas(32) mem_chunk_t {
    static const size_t byte_alignment = 32;
    float *data;

    mem_chunk_t (size_t bytes) {
        data = reinterpret_cast<float*>(boost::alignment::aligned_alloc(byte_alignment, bytes));
    }

    ~mem_chunk_t() {
        boost::alignment::aligned_free(data);
        data = nullptr;
    }
};

class ChunkManager {
private:
    std::vector<std::unique_ptr<mem_chunk_t>> chunks;
public:
    ChunkManager() {}
    ~ChunkManager() {
        chunks.clear();
    }

    mem_chunk_t* allocate(size_t bytes) {
        auto chunk = std::make_unique<mem_chunk_t>(bytes);
        const auto ptr = chunk.get();
        chunks.push_back(std::move(chunk));
        return ptr;
    }

    mem_chunk_t* get(size_t n) const {
        return chunks.at(n).get();
    }

    inline size_t size() const { return chunks.size(); }
};

struct vector_t {
    alignas(32) float* v;

    vector_t (size_t n) {
        v = reinterpret_cast<float*>(boost::alignment::aligned_alloc(32, n*4));
    }

    ~vector_t() {
        boost::alignment::aligned_free(v);
        v = nullptr;
    }
};

static inline constexpr size_t kilobyte(const size_t n) {
    return n * 1024UL;
}

static inline constexpr size_t megabyte(const size_t n) {
    return kilobyte(n * 1024UL);
}

int what() {
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
    const auto chunk_size = megabyte(32);

    // To simplify experiments, we require the block to exactly match our expectations
    // about vector lengths. Put differently, all bytes in the buffer can be used.
    static_assert((chunk_size % (sizeof(float)*N)) == 0);

    mem_chunk_t* chunk_a = nullptr;
    mem_chunk_t* chunk_b = nullptr;
    size_t remaining_chunk_size = 0;    // number of remaining bytes in the current chunk
    size_t float_offset = 0;            // index into the current buffer, counts floats

    // Keep track of the total sum for validation.
    auto expected_total_sum = 0.0f;

    for (size_t j = 0; j < M; ++j) {

        // Initial condition, also reached during runtime: If one block is full,
        // allocate another one.
        if (remaining_chunk_size == 0) {
            std::cout << "Allocating chunk." << std::endl;

            chunk_a = chunkManager_a.allocate(chunk_size);
            chunk_b = chunkManager_b.allocate(chunk_size);
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

        size_t current_chunk = 0;
        chunk_a = chunkManager_a.get(current_chunk);
        chunk_b = chunkManager_b.get(current_chunk);
        float_offset = 0;

        const auto vectors_per_chunk = chunk_size / (N * sizeof(float));
        auto remaining_vectors_per_chunk = vectors_per_chunk;

        auto start_time = std::chrono::high_resolution_clock::now();

        // Run for a couple of test iterations ...
        for (size_t vector_idx = 0; vector_idx < M; ++vector_idx) {

            if (remaining_vectors_per_chunk == 0) {
                current_chunk += 1;
                chunk_a = chunkManager_a.get(current_chunk);
                chunk_b = chunkManager_b.get(current_chunk);

                float_offset = 0;
                remaining_vectors_per_chunk = vectors_per_chunk;
            }

            auto a_row = &chunk_a->data[float_offset];
            auto b_row = &chunk_b->data[float_offset];

            --remaining_vectors_per_chunk;
            float_offset += N;

            result[vector_idx] = 0.0f;

            // Calculate the dot product of the 2048-element vector
            auto total = _mm256_set1_ps(0.0f);
            static_assert((N & 31) == 0, "Vector length must be a multiple of 32 elements.");
            for (size_t i = 0; i < N; i += 32) {
                // Prefetch the next batch into L2 - saves around 40ms on 2 million 2048-float rows.
                _mm_prefetch(&a_row[i + 32 * 8], _MM_HINT_T1);
                _mm_prefetch(&b_row[i + 32 * 8], _MM_HINT_T1);

                // load 32 floats per vector
                // For some reason, _mm256_loadu_ps is faster than _mm256_load_ps on both AVX and AVX2 ...
                // In this example, times drop from 160ms to 120ms ... ?
                const auto a0 = _mm256_load_ps(&a_row[i]);
                const auto b0 = _mm256_load_ps(&b_row[i]);
                const auto a1 = _mm256_load_ps(&a_row[i + 8]);
                const auto b1 = _mm256_load_ps(&b_row[i + 8]);
                const auto a2 = _mm256_load_ps(&a_row[i + 16]);
                const auto b2 = _mm256_load_ps(&b_row[i + 16]);
                const auto a3 = _mm256_load_ps(&a_row[i + 24]);
                const auto b3 = _mm256_load_ps(&b_row[i + 24]);

                // do separate dot products
                const auto c0 = _mm256_dp_ps(a0, b0, 0xff);
                const auto c1 = _mm256_dp_ps(a1, b1, 0xff);
                const auto c2 = _mm256_dp_ps(a2, b2, 0xff);
                const auto c3 = _mm256_dp_ps(a3, b3, 0xff);

                // do separate partial sums
                const auto p0 = _mm256_add_ps(c0, c1);
                const auto p1 = _mm256_add_ps(c2, c3);

                // aggregate the partial sums and allocate to running total
                const auto s = _mm256_add_ps(p0, p1);
                total = _mm256_add_ps(total, s);
            }

            alignas(32) float ptr[8];
            _mm256_store_ps(ptr, total);
            result[vector_idx] = ptr[0] + ptr[5];
            total_sum += ptr[0] + ptr[5];
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