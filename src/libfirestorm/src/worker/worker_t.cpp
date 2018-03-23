//
// Created by sunside on 23.03.18.
//

#include <cassert>
#include <firestorm/engine/worker/worker_t.h>

namespace firestorm {

    boost::optional<std::weak_ptr<const mem_chunk_t>> worker_t::unassign_chunk() {
        if (_assigned_chunks.empty()) {
            return boost::none;
        }

        auto chunk = _assigned_chunks.front();
        _assigned_chunks.pop_front();
        return chunk;
    }

    size_t worker_t::accept(const mapper_t& visitor, combine_t& combiner, const vector_t &query) const {
        size_t vectors_processed = 0;

        for (const auto &chunk : _assigned_chunks) {
            const auto shared_chunk = chunk.lock();
            if (shared_chunk == nullptr) continue;

            const auto &chunk_ref = *shared_chunk;
            assert(chunk_ref.dimensions == query.dimensions);

            auto result = visitor.map(chunk_ref, query);
            combiner.combine(result);

            // TODO: We can send the number of processed vectors along with the mapper results
            vectors_processed += chunk_ref.vectors;
        }

        return vectors_processed;
    }

}