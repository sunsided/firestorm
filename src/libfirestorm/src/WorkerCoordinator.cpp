//
// Created by sunside on 04.03.18.
//

#include <thread>
#include <vector>
#include <firestorm/Worker.h>
#include <firestorm/WorkerCoordinator.h>
#include "ThreadedWorker.h"

using namespace std;

class WorkerCoordinator::Impl {
public:
    explicit Impl() noexcept = default;

    void addWorker() {
        // "emplace_back might leak if vector cannot extended"
        // https://stackoverflow.com/a/15784982/195651
        workers.push_back(make_unique<ThreadedWorker>(make_unique<Worker>()));
    }

    void assign_chunk(weak_ptr<const mem_chunk_t> chunk) {
        assert(!workers.empty());

        // TODO: We might want to enforce to have nearby chunk addresses in each worker so that pre-fetching might bring in the next chunk already
        // TODO: We need to rebalance chunks if chunks get evicted by the manager.
        ThreadedWorker* target = nullptr;
        size_t count = 0;
        for(auto& worker : workers) {
            if (worker->num_chunks() <= count) continue;
            target = worker.get();
        }

        assert(target != nullptr);
        target->assign_chunk(chunk);
    }

private:
    vector<unique_ptr<ThreadedWorker>> workers;
};

WorkerCoordinator::WorkerCoordinator(size_t workerCount) noexcept
    : impl{make_unique<Impl>()}
{
    // TODO: Throw if stupid
    workerCount = workerCount > 0 ? workerCount : 1;
    for (size_t i = 0; i < workerCount; ++i) {
        impl->addWorker();
    }
}

WorkerCoordinator::~WorkerCoordinator() = default;
WorkerCoordinator::WorkerCoordinator(WorkerCoordinator&&) noexcept = default;
WorkerCoordinator& WorkerCoordinator::operator=(WorkerCoordinator&&) noexcept = default;

void WorkerCoordinator::assign_chunk(std::weak_ptr<const mem_chunk_t> chunk) const {
    impl->assign_chunk(std::move(chunk));
}