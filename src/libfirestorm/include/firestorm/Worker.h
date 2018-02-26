//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_WORKER_H
#define FIRESTORM_WORKER_H

#include <deque>
#include <map>
#include <boost/optional.hpp>
#include <utility>
#include "mem_chunk_t.h"
#include "ChunkVisitor.h"
#include "ChunkAccessor.h"
#include "result_t.h"

/// Worker that processes vectors in the registered chunks.
class Worker {
private:
    /// Ideally, subsequently allocated chunks appear sequentially in memory.
    /// To aid in processing, balancing between workers should be from "right to left",
    /// such that adding an item from a worker to the "right" at the back
    /// moves the first item (on the front) to a worker on the "left". This way,
    /// memory is quasi-sequential for each worker.
    std::deque<chunk_idx_t > assigned_chunks;

    const std::shared_ptr<const ChunkAccessor> accessor;

public:
    /// Initializes an instance of the Worker class.
    /// \param accessor An accessor to a chunk manager.
    explicit Worker(std::shared_ptr<const ChunkAccessor> accessor)
            : assigned_chunks{}, accessor{std::move(accessor)}
    {}

    /// Determines whether this worker has chunks assigned.
    /// \return true if there are chunks registered for processing.
    inline bool has_work() const { return num_chunks() > 0; }

    /// Determines the number of assigned chunks.
    /// \return The number of assigned chunks.
    inline size_t num_chunks() const { return assigned_chunks.size(); }

    /// Assigns a chunk of the manager to this worker.
    /// \param chunk_idx The index of the chunk to process.
    void assign_chunk(chunk_idx_t chunk_idx) {
        assigned_chunks.push_back(chunk_idx);
    }

    /// Unassigns a chunk of the manager from this worker.
    /// \return The index of the chunk that was unassigned.
    boost::optional<chunk_idx_t> unassign_chunk() {
        if (assigned_chunks.empty()) {
            return boost::none;
        }

        auto chunk = assigned_chunks.front();
        assigned_chunks.pop_front();
        return chunk;
    }

    /// Creates a result buffer that is able to hold results for all vector mappings of this manager.
    /// \return The result buffer.
    std::map<size_t, std::shared_ptr<result_t>> create_result_buffer() const {
        std::map<size_t, std::shared_ptr<result_t>> results;
        for(auto chunk_idx : assigned_chunks) {
            auto shared_chunk = accessor->get_ro(chunk_idx);
            if (shared_chunk == nullptr) continue;

            const auto& chunk_ptr = *shared_chunk;
            results[chunk_ptr.index] = std::make_shared<result_t>(chunk_ptr.index, chunk_ptr.vectors);
        }
        return results;
    }

    /// Applies the specified visitor with the given query vector to the registered chunks.
    /// \param visitor The visitor to use.
    /// \param query The query vector to operate on.
    /// \param results The result buffer.
    /// \return The number of vectors that were processed.
    size_t accept(const ChunkVisitor& visitor, const vector_t& query, std::map<size_t, std::shared_ptr<result_t>>& results) const {
        size_t vectors_processed = 0;
        for (const auto chunk : assigned_chunks) {
            const auto shared_chunk = accessor->get_ro(chunk);
            if (shared_chunk == nullptr) continue;

            const auto& chunk_ref = *shared_chunk;
            assert(chunk_ref.dimensions == query.dimensions);

            auto result = results[chunk_ref.index];
            assert(result != nullptr);
            visitor.visit(chunk_ref, query, result->scores);

            vectors_processed += chunk_ref.vectors;
        }
        return vectors_processed;
    }
};

#endif //FIRESTORM_WORKER_H
