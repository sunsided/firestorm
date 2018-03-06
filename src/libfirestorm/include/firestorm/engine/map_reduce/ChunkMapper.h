//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_CHUNKVISITOR_H
#define FIRESTORM_CHUNKVISITOR_H

#include <any>
#include <memory>
#include "firestorm/engine/types/mem_chunk_t.h"
#include "firestorm/engine/types/vector_t.h"
#include "firestorm/engine/types/score_t.h"

namespace firestorm {

    class ChunkMapper {
    public:
        virtual ~ChunkMapper() = default;

        /// Maps a chunk according to the logic of the visitor.
        /// \param chunk The chunk to map.
        /// \param query The query vector to use.
        virtual std::any map(const mem_chunk_t &chunk, const vector_t &query) = 0;
    };

    /// A stateful class that performs result combination.
    class ChunkCombiner {
    public:
        virtual ~ChunkCombiner() = default;

        /// Initializes a combining operation.
        virtual void begin() = 0;

        /// Combines the results of two mappers.
        /// \param other The other visitor to merge into the local results.
        /// \return The combined result.
        virtual void combine(const std::any& other) = 0;

        /// Finalizes a combining operation.
        /// \return The combined result.
        virtual std::any finish() = 0;
    };

}

#endif //FIRESTORM_CHUNKVISITOR_H
