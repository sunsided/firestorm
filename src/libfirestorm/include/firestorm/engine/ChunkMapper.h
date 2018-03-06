//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_CHUNKVISITOR_H
#define FIRESTORM_CHUNKVISITOR_H

#include <memory>
#include "mem_chunk_t.h"
#include "vector_t.h"
#include "score_t.h"

using chunk_visitor_result_t = std::shared_ptr<void>;

class ChunkMapper {
public:
    /// Resets this visitor to its default state.
    ///
    /// This is called before a sequence of maps.
    virtual void map_prepare() {};

    /// Maps a chunk according to the logic of the visitor.
    /// \param chunk The chunk to map.
    /// \param query The query vector to use.
    virtual void map(const mem_chunk_t &chunk, const vector_t &query) = 0;

    /// Finalizes all results from this visitor.
    ///
    /// This is called after a sequence of maps but before merging results.
    virtual void map_done() {};

    /// Combines the results of two visitors.
    /// \param other The other visitor to merge into the local results.
    virtual void combine(const ChunkMapper& other) = 0;
};

#endif //FIRESTORM_CHUNKVISITOR_H
