//
// Created by Markus on 28.05.2017.
//

#ifndef FIRESTORM_CHUNKACCESSOR_H
#define FIRESTORM_CHUNKACCESSOR_H

#include <memory>
#include "mem_chunk_t.h"
#include "chunk_idx_t.h"

class ChunkAccessor {
public:
    virtual std::shared_ptr<const mem_chunk_t> get_ro(const chunk_idx_t chunk_idx) const = 0;
};

#endif //FIRESTORM_CHUNKACCESSOR_H
