//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_CHUNKMANAGER_H
#define FIRESTORM_CHUNKMANAGER_H

#include <vector>
#include <memory>
#include "ChunkAccessor.h"

namespace firestorm {

/// This class implements memory chunk handling for vectors
/// stored in consecutive memory.
    class ChunkManager : public ChunkAccessor {
    public:
        ChunkManager() = default;

        virtual ~ChunkManager() = default;

        /// Allocates a new memory chunk for the specified amount of vectors of the given dimensionality.
        /// \param vectors The number of vectors to store in this chunk.
        /// \param dimensions The dimensionality of the vectors in this chunk.
        /// \return A pointer to a memory chunk.
        std::shared_ptr<mem_chunk_t> allocate(const size_t vectors, const size_t dimensions) {
            auto chunk = std::make_shared<mem_chunk_t>(next_index, vectors, dimensions);
            ++next_index;

            chunks.push_back(chunk);
            return chunk;
        }

        /// Obtains a memory chunk for read-only access.
        /// \param n The index of the chunk to obtain.
        /// \return A pointer to the chunk.
        std::shared_ptr<const mem_chunk_t> get_ro(const chunk_idx_t n) const override {
            // TODO: Exception handling for invalid n
            return chunks.at(n);
        }

        /// Obtains a memory chunk for read and write access.
        /// \param n The index of the chunk to obtain.
        /// \return A pointer to the chunk.
        std::shared_ptr<mem_chunk_t> get_rw(const chunk_idx_t n) const {
            // TODO: Exception handling for invalid n
            return chunks.at(n);
        }

        /// Gets the number of chunks registered in this manager.
        /// \return The number of chunks.
        inline size_t size() const { return chunks.size(); }

    private:
        std::vector<std::shared_ptr<mem_chunk_t>> chunks;
        chunk_idx_t next_index = 0;
    };

}

#endif //FIRESTORM_CHUNKMANAGER_H
