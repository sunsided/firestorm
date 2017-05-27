//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_CHUNKMANAGER_H
#define FIRESTORM_CHUNKMANAGER_H

#include <vector>
#include <memory>
#include "mem_chunk_t.h"

class ChunkManager {
private:
    std::vector<std::shared_ptr<mem_chunk_t>> chunks;
    chunk_idx_t next_index = 0;
public:
    ChunkManager() {}
    ~ChunkManager() {
        chunks.clear();
    }

    std::weak_ptr<mem_chunk_t> allocate(const bytes_t bytes) {
        auto chunk = std::make_shared<mem_chunk_t>(next_index, bytes);
        ++next_index;

        const std::weak_ptr<mem_chunk_t> ptr = chunk;
        chunks.push_back(std::move(chunk));
        return ptr;
    }

    std::weak_ptr<mem_chunk_t> const get(const chunk_idx_t n) const {
        return chunks.at(n);
    }

    inline size_t size() const { return chunks.size(); }
};

#endif //FIRESTORM_CHUNKMANAGER_H
