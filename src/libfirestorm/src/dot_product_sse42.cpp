//
// Created by sunside on 11.02.18.
//

#if SSE_VERSION == 4

#include <memory>
#include <smmintrin.h>
#include "firestorm/dot_product_sse42.h"

// https://stackoverflow.com/a/35270026/195651

float dot_product_sse42(const float *const __restrict__ a_row, const float *const __restrict__ b_row, const size_t N) noexcept {
    auto total = _mm_setzero_ps();
    for (size_t i = 0; i < N; i += 4) {
        const auto a0 = _mm_load_ps(&a_row[i]);
        const auto b0 = _mm_load_ps(&b_row[i]);

        const auto result = _mm_dp_ps(a0, b0, 0xff);
        total = _mm_add_ps(total, result);
    }

    alignas(16) float ptr[8];
    _mm_store_ps(ptr, total);
    return ptr[0];
}

#endif //SSE_VERSION