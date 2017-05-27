//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_CHUNKVISITOR_H
#define FIRESTORM_CHUNKVISITOR_H

#include <memory>
#include "mem_chunk_t.h"
#include "vector_t.h"

class ChunkVisitor {
public:
    virtual void visit(const mem_chunk_t& chunk, const vector_t& query) const = 0;
};

#endif //FIRESTORM_CHUNKVISITOR_H
