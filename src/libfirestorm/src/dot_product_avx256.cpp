//
// Created by Markus on 28.05.2017.
//


#ifdef USE_AVX

#include <cstddef>
#include <firestorm/engine/simd.h>
#include <firestorm/engine/dot_product_avx256.h>

using namespace std;

float dot_product_avx256(const float *const __restrict__ a_row, const float *const __restrict__ b_row, const size_t N) noexcept {
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

float vec_norm_avx256(const float *const a_row, const size_t N) noexcept {
    // TODO: Ideally we can hadd the fields directly and stay in the AVX registers
    // TODO: check wekan for note on hadd alternative (shuffle+vadd)
    const auto squared_norm = dot_product_avx256(a_row, a_row, N);
    auto v = _mm256_set1_ps(squared_norm);

    const auto norm = _mm256_sqrt_ps(v);
    alignas(32) float ptr[8];
    _mm256_store_ps(ptr, norm);

    return ptr[0];
}

float vec_normalize_avx256(float *const a_row, const size_t N) noexcept {
    const auto norm = vec_norm_avx256(a_row, N);
    if (norm < FLOAT_ZERO_EPSILON && norm > -FLOAT_ZERO_EPSILON) {
        return 0.0;
    }

    auto n = _mm256_set1_ps(1.0f/norm);
    for (size_t i = 0; i < N; i += 32) {
        // Prefetch the next batch into L2 - saves around 40ms on 2 million 2048-float rows.
        _mm_prefetch(&a_row[i + 32 * 8], _MM_HINT_T1);

        // Load 32 floats per vector.
        const auto a0 = _mm256_load_ps(&a_row[i]);
        const auto a1 = _mm256_load_ps(&a_row[i + 8]);
        const auto a2 = _mm256_load_ps(&a_row[i + 16]);
        const auto a3 = _mm256_load_ps(&a_row[i + 24]);

        // Normalize the values individually.
        const auto c0 = _mm256_mul_ps(a0, n);
        const auto c1 = _mm256_mul_ps(a1, n);
        const auto c2 = _mm256_mul_ps(a2, n);
        const auto c3 = _mm256_mul_ps(a3, n);

        // Write back the values.
        _mm256_store_ps(&a_row[i], c0);
        _mm256_store_ps(&a_row[i + 8], c1);
        _mm256_store_ps(&a_row[i + 16], c2);
        _mm256_store_ps(&a_row[i + 24], c3);
    }

    return norm;
}

#endif
