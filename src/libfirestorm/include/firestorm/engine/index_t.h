//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_INDEX_T_H
#define FIRESTORM_INDEX_T_H

#include "chunk_idx_t.h"
#include "vector_idx_t.h"

namespace firestorm {

/// An index entry.
    struct index_t {
        constexpr index_t(const chunk_idx_t chunk, const vector_idx_t index)
                : chunk(chunk), index(index) {}

        /// Identifies the chunk in the manager.
        const chunk_idx_t chunk;
        /// Identifies the index in the chunk.
        const vector_idx_t index;
    };

}

#endif //FIRESTORM_INDEX_T_H
