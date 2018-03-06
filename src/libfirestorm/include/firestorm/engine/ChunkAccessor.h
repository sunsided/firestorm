//
// Created by Markus on 28.05.2017.
//

#ifndef FIRESTORM_CHUNKACCESSOR_H
#define FIRESTORM_CHUNKACCESSOR_H

#include <memory>
#include "mem_chunk_t.h"
#include "chunk_idx_t.h"

namespace firestorm {

/// This class implements a read-only accessor to memory chunks.
    class ChunkAccessor {
    public:
        /// Obtains a memory chunk for read-only access.
        /// \param n The index of the chunk to obtain.
        /// \return A pointer to the chunk.
        virtual std::shared_ptr<const mem_chunk_t> get_ro(chunk_idx_t chunk_idx) const = 0;
    };

}

#endif //FIRESTORM_CHUNKACCESSOR_H
