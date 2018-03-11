//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_INDEX_T_H
#define FIRESTORM_INDEX_T_H

#include "firestorm/engine/types/chunk_idx_t.h"
#include "vector_idx_t.h"

namespace firestorm {

    /// An index entry.
    struct index_t {
        constexpr index_t(const chunk_idx_t chunk, const vector_idx_t index)
                : _chunk(chunk), _index(index)
        {}

        constexpr index_t(const index_t& other)
                : _chunk(other._chunk), _index(other._index)
        {}

        constexpr index_t& operator=(const index_t& other)
        {
            _index = other._index;
            _chunk = other._chunk;
            return *this;
        }

        inline bool operator==(const index_t &b) const {
            return _chunk == b._chunk && _index == b._index;
        }

        inline bool operator!=(const index_t &b) const {
            return !(*this == b);
        }

        inline chunk_idx_t chunk() const { return _chunk; }
        inline vector_idx_t vector_index() const { return _index; }

    private:
        /// Identifies the chunk in the manager.
        chunk_idx_t _chunk;
        /// Identifies the index in the chunk.
        vector_idx_t _index;
    };

}

#endif //FIRESTORM_INDEX_T_H
