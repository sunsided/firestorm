#include <iostream>
#include <random>
#include <chrono>
#include <functional>
#include <boost/align/aligned_alloc.hpp>
#include "Simd.h"

// TODO: Boost
// TODO: Boost.SIMD


// TODO: Number of elements needs to be identical for all vectors, so is identical in whole table.
// TODO: Indexing can be performed on a separate instance.

union alignas(32) float8 {
    float f[8];
};

union alignas(32) float32 {
    float f[32];
    float8 f8[4];
};

// AVX:
//  - registers YMM0 to YMM15, each 256 bit / 32 byte / 8 floats
//  - three-operand form (c = a OP b), so three registers per operation
//  - allows for five operations (5 * 3 registers = 15 registers) on 8x8 floats each
//  - Haswell allows for 32 byte reads into L1

void foo() {
    float8 m0 {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    float8 m1 {1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f};
    float8 m2 {2.0f, 2.0f, 2.0f, 2.0f, 4.0f, 4.0f, 4.0f, 4.0f};
    float8 m3 {1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f};

    __m256 a0 = _mm256_loadu_ps(m0.f);
    __m256 b0 = _mm256_loadu_ps(m0.f);

    __m256 a1 = _mm256_loadu_ps(m0.f);
    __m256 b1 = _mm256_loadu_ps(m1.f);

    __m256 a2 = _mm256_loadu_ps(m2.f);
    __m256 b2 = _mm256_loadu_ps(m3.f);

    __m256 a3 = _mm256_loadu_ps(m1.f);
    __m256 b3 = _mm256_loadu_ps(m3.f);

    __m256 c0 = _mm256_dp_ps(a0, b0, 0b11110001);
    __m256 c1 = _mm256_dp_ps(a1, b1, 0b11110101);
    __m256 c2 = _mm256_dp_ps(a2, b2, 0b11111111);
    __m256 c3 = _mm256_dp_ps(a3, b3, 0b11110000);

    _mm256_store_ps(m0.f, c0);
    _mm256_store_ps(m1.f, c1);
    _mm256_store_ps(m2.f, c2);
    _mm256_store_ps(m3.f, c3);

    std::cout << "Alignment offset: " << (reinterpret_cast<size_t>(&m0.f[0]) & 31L) << std::endl;
    std::cout << m0.f[0] << " size " << sizeof(m0) << std::endl;
}

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

int what() {

    const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(0.0f, 2.0f);
    auto random = std::bind(distribution, generator);

    const size_t N = 2048;
    const size_t M = 10000;

    auto b = new vector_t*[M];
    auto a = new vector_t*[M];
    auto expected = new float[M];
    auto sum = new float[M];

    std::cout << "Initializing vectors ..." << std::endl;
    for (size_t j = 0; j < M; ++j) {
        if (j % 2500 == 0) {
            std::cout << "- " << j << "/" << M << std::endl;
        }

        expected[j] = 0;
        a[j] = new vector_t(N);
        b[j] = new vector_t(N);

        for (size_t i = 0; i < N; ++i) {
            a[j]->v[i] = random();
            b[j]->v[i] = random();
            expected[j] += a[j]->v[i] * b[j]->v[i];
        }
    }

    std::cout << "Running tests ..." << std::endl;
    for (size_t repetition = 0; repetition < 20; ++repetition)
    {
        auto start_time = std::chrono::high_resolution_clock::now();
        for (size_t test_iteration = 0; test_iteration < M; ++test_iteration) {
            sum[test_iteration] = 0.0f;
            float* a_row = a[test_iteration]->v;
            float* b_row = b[test_iteration]->v;

            auto total = _mm256_set1_ps(0.0f);

            static_assert((N & 31) == 0, "Vector length must be a multiple of 32 elements.");
            for (size_t i = 0; i < N; i += 32) {
                // prefetch the next batch into L2 - saves around 40ms on 2 million 2048-float rows
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

                // aggregate the partial sums and add to running total
                const auto s = _mm256_add_ps(p0, p1);
                total = _mm256_add_ps(total, s);
            }

            alignas(32) float ptr[8];
            _mm256_store_ps(ptr, total);
            sum[test_iteration] = ptr[0] + ptr[5];
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        std::cout << "Sum: " << sum[0] << " (expected: " << expected[0] << ")" << " - duration: " << duration << "ms"
                  << std::endl;
    }

    std::cout << "Cleaning up ..." << std::endl;
    for (size_t j = 0; j < M; ++j) {
        delete a[j];
        delete b[j];
    }
    delete[] a;
    delete[] b;
    delete expected;
    delete sum;

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