//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_WORKER_H
#define FIRESTORM_WORKER_H

#include <deque>
#include <map>
#include <memory>
#include <utility>
#include <boost/optional.hpp>
#include "mem_chunk_t.h"
#include "ChunkMapper.h"
#include "ChunkAccessor.h"
#include "result_t.h"

namespace firestorm {

    /// Worker that processes vectors in the registered chunks.
    class Worker {
    private:
        /// Ideally, subsequently allocated chunks appear sequentially in memory.
        /// To aid in processing, balancing between workers should be from "right to left",
        /// such that adding an item from a worker to the "right" at the back
        /// moves the first item (on the front) to a worker on the "left". This way,
        /// memory is quasi-sequential for each worker.
        std::deque<std::weak_ptr<const mem_chunk_t>> assigned_chunks;

    public:
        /// Initializes an instance of the Worker class.
        /// \param accessor An accessor to a chunk manager.
        explicit Worker()
                : assigned_chunks{} {}

        /// Determines whether this worker has chunks assigned.
        /// \return true if there are chunks registered for processing.
        inline bool has_work() const { return num_chunks() > 0; }

        /// Determines the number of assigned chunks.
        /// \return The number of assigned chunks.
        inline size_t num_chunks() const { return assigned_chunks.size(); }

        /// Assigns a chunk of the manager to this worker.
        /// \param chunk_idx The index of the chunk to process.
        void assign_chunk(std::weak_ptr<const mem_chunk_t> chunk) {
            assigned_chunks.push_back(chunk);
        }

        /// Unassigns a chunk of the manager from this worker.
        /// \return The index of the chunk that was unassigned.
        boost::optional<std::weak_ptr<const mem_chunk_t>> unassign_chunk() {
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
            for (auto chunk : assigned_chunks) {
                auto shared_chunk = chunk.lock();
                if (shared_chunk == nullptr) continue;

                const auto &chunk_ptr = *shared_chunk;
                results[chunk_ptr.index] = std::make_shared<result_t>(chunk_ptr.index, chunk_ptr.vectors);
            }
            return results;
        }

        /// Applies the specified visitor with the given query vector to the registered chunks.
        /// \param visitor The visitor to use.
        /// \param query The query vector to operate on.
        /// \param results The result buffer.
        /// \return The number of vectors that were processed.
        size_t accept(ChunkMapper &visitor, const vector_t &query) const {

            size_t vectors_processed = 0;
            visitor.map_prepare();

            for (const auto chunk : assigned_chunks) {
                const auto shared_chunk = chunk.lock();
                if (shared_chunk == nullptr) continue;

                const auto &chunk_ref = *shared_chunk;
                assert(chunk_ref.dimensions == query.dimensions);

                visitor.map(chunk_ref, query);
                vectors_processed += chunk_ref.vectors;
            }

            visitor.map_done();

            return vectors_processed;
        }
    };

}

#endif //FIRESTORM_WORKER_H
