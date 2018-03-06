//
// Created by Markus on 28.05.2017.
//

#ifndef FIRESTORM_RESULT_T_H
#define FIRESTORM_RESULT_T_H

#include <memory>
#include "chunk_idx_t.h"
#include "score_t.h"

namespace firestorm {

    struct result_t {
        result_t(const chunk_idx_t chunk_idx, const size_t num_vectors) noexcept
                : chunk(chunk_idx), scores(num_vectors) {}

        result_t(const result_t &other) = delete;

        result_t(result_t &&other) noexcept
                : chunk(other.chunk), scores(std::move(other.scores)) {}

        const chunk_idx_t chunk;
        std::vector<score_t> scores;
    };

}

#endif //FIRESTORM_RESULT_T_H
