//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_CHUNKVISITOR_H
#define FIRESTORM_CHUNKVISITOR_H

#include <memory>
#include "mem_chunk_t.h"

class ChunkVisitor {
public:
    void visit(const std::shared_ptr<const mem_chunk_t> chunk)
    {}
};

#endif //FIRESTORM_CHUNKVISITOR_H
