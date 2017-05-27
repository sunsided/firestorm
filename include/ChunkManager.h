//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_CHUNKMANAGER_H
#define FIRESTORM_CHUNKMANAGER_H

#include <vector>
#include <memory>
#include "mem_chunk.h"

class ChunkManager {
private:
    std::vector<std::unique_ptr<mem_chunk_t>> chunks;
public:
    ChunkManager() {}
    ~ChunkManager() {
        chunks.clear();
    }

    mem_chunk_t* allocate(bytes_t bytes) {
        auto chunk = std::make_unique<mem_chunk_t>(bytes);
        const auto ptr = chunk.get();
        chunks.push_back(std::move(chunk));
        return ptr;
    }

    mem_chunk_t* get(size_t n) const {
        return chunks.at(n).get();
    }

    inline size_t size() const { return chunks.size(); }
};

#endif //FIRESTORM_CHUNKMANAGER_H
