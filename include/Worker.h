//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_WORKER_H
#define FIRESTORM_WORKER_H

#include <deque>
#include <map>
#include <boost/optional.hpp>
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
    Worker(const std::shared_ptr<const ChunkAccessor>& accessor) : accessor(accessor) {}

    void assign_chunk(chunk_idx_t chunk_idx) {
        assigned_chunks.push_back(chunk_idx);
    }

    boost::optional<chunk_idx_t> unassign_chunk() {
        if (assigned_chunks.empty()) {
            return boost::none;
        }

        auto chunk = assigned_chunks.front();
        assigned_chunks.pop_front();
        return chunk;
    }

    std::map<size_t, std::shared_ptr<result_t>> create_result_buffer() const {
        std::map<size_t, std::shared_ptr<result_t>> results;
        for(auto chunk : assigned_chunks) {
            auto shared_chunk = accessor->get_ro(chunk);
            if (shared_chunk == nullptr) continue;

            const auto chunk_ptr = shared_chunk.get();
            results[chunk_ptr->index] = std::make_shared<result_t>(chunk_ptr->index, chunk_ptr->vectors);
        }
        return results;
    }

    std::vector<std::shared_ptr<result_t>> accept(ChunkVisitor& visitor, const vector_t& query) {
        std::vector<std::shared_ptr<result_t>> results;
        for(auto chunk : assigned_chunks) {
            auto shared_chunk = accessor->get_ro(chunk);
            if (shared_chunk == nullptr) continue;

            const auto chunk_ptr = shared_chunk.get();
            assert(chunk_ptr->dimensions == query.dimensions);

            auto result = std::make_shared<result_t>(chunk_ptr->index, chunk_ptr->vectors);
            visitor.visit(*chunk_ptr, query, result->scores);
            results.push_back(std::move(result));
        }
        return results;
    }

    void accept(ChunkVisitor& visitor, const vector_t& query, std::map<size_t, std::shared_ptr<result_t>>& results) {
        for (const auto chunk : assigned_chunks) {
            const auto shared_chunk = accessor->get_ro(chunk);
            if (shared_chunk == nullptr) continue;

            const auto chunk_ptr = shared_chunk.get();
            assert(chunk_ptr->dimensions == query.dimensions);

            auto result = results[chunk_ptr->index];
            visitor.visit(*chunk_ptr, query, result->scores);
        }
    }
};

#endif //FIRESTORM_WORKER_H
