//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_WORKER_H
#define FIRESTORM_WORKER_H

#include <deque>
#include <map>
#include <memory>
#include <utility>
#include <shared_mutex>
#include <boost/optional.hpp>
#include "firestorm/engine/types/mem_chunk_t.h"
#include "firestorm/engine/mapreduce/mapper_t.h"
#include "firestorm/engine/mapreduce/combiner_t.h"
#include "firestorm/engine/memory/chunk_accessor.h"
#include "firestorm/engine/types/result_t.h"

namespace firestorm {

    /// Worker that processes vectors in the registered chunks.
    class worker_t {
    public:
        /// Initializes an instance of the Worker class.
        /// \param accessor An accessor to a chunk manager.
        explicit worker_t()
                : _assigned_chunks{} {}

        /// Determines whether this worker has chunks assigned.
        /// \return true if there are chunks registered for processing.
        inline bool has_work() const { return num_chunks() > 0; }

        /// Determines the number of assigned chunks.
        /// \return The number of assigned chunks.
        inline size_t num_chunks() const { return _assigned_chunks.size(); }

        /// Assigns a chunk of the manager to this worker.
        /// \param chunk_idx The index of the chunk to process.
        inline void assign_chunk(const std::weak_ptr<const mem_chunk_t> &chunk) {
            _assigned_chunks.push_back(chunk);
        }

        /// \brief Unassigns all chunks from this worker.
        inline void unassign_all_chunks() {
            _assigned_chunks.clear();
        }

        /// Unassigns a chunk of the manager from this worker.
        /// \return The index of the chunk that was unassigned.
        boost::optional<std::weak_ptr<const mem_chunk_t>> unassign_chunk();

        /// Applies the specified visitor with the given query vector to the registered chunks.
        /// \param visitor The visitor to use.
        /// \param reducer The reducer to use.
        /// \param query The query vector to operate on.
        /// \param results The result buffer.
        /// \return The number of vectors that were processed.
        size_t accept(const mapper_t& visitor, combine_t& combiner, const vector_t &query) const;

    private:
        /// Ideally, subsequently allocated chunks appear sequentially in memory.
        /// To aid in processing, balancing between workers should be from "right to left",
        /// such that adding an item from a worker to the "right" at the back
        /// moves the first item (on the front) to a worker on the "left". This way,
        /// memory is quasi-sequential for each worker.
        std::deque<std::weak_ptr<const mem_chunk_t>> _assigned_chunks;
    };

}

#endif //FIRESTORM_WORKER_H
