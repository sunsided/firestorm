//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_WORKER_H
#define FIRESTORM_WORKER_H

#include <deque>
#include "mem_chunk_t.h"

/// Worker that processes vectors in the registered chunks.
class Worker {
private:
    /// Ideally, subsequently allocated chunks appear sequentially in memory.
    /// To aid in processing, balancing between workers should be from "right to left",
    /// such that adding an item from a worker to the "right" at the back
    /// moves the first item (on the front) to a worker on the "left". This way,
    /// memory is quasi-sequential for each worker.
    std::deque<std::weak_ptr<mem_chunk_t>> assigned_blocks;

public:
    void assign_block(std::weak_ptr<mem_chunk_t> block) {
        assigned_blocks.push_back(block);
    }

    std::weak_ptr<mem_chunk_t> unassign_block() {
        auto block = std::move(assigned_blocks.front());
        assigned_blocks.pop_front();
        return block;
    }

    // TODO: Implement visitor pattern
};

#endif //FIRESTORM_WORKER_H
