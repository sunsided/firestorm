//
// Created by sunside on 04.03.18.
//

#include <utility>
#include <vector>
#include <firestorm/Worker.h>
#include <firestorm/WorkerCoordinator.h>

using namespace std;

class WorkerCoordinator::Impl {
public:
    explicit Impl() noexcept
        : assignment_idx(0)
    {}

    void addWorker() {
        // "emplace_back might leak if vector cannot extended"
        // https://stackoverflow.com/a/15784982/195651
        workers.push_back(make_unique<Worker>());

        // TODO: Bring up a worker thread
        // TODO: Configure the input queue
        // TODO: present results via promise/future
    }

    void assignChunk(weak_ptr<const mem_chunk_t> chunk) {
        assignment_idx = (assignment_idx++) % workers.size();
        workers.at(assignment_idx)->assign_chunk(std::move(chunk));
    }

private:
    size_t assignment_idx;
    vector<unique_ptr<Worker>> workers;
};

WorkerCoordinator::WorkerCoordinator(size_t initialCount) noexcept
    : impl{make_unique<Impl>()}
{
    for (size_t i = 0; i < initialCount; ++i) {
        impl->addWorker();
    }
}

WorkerCoordinator::~WorkerCoordinator() = default;
WorkerCoordinator::WorkerCoordinator(WorkerCoordinator&&) noexcept = default;
WorkerCoordinator& WorkerCoordinator::operator=(WorkerCoordinator&&) noexcept = default;

void WorkerCoordinator::assignChunk(std::weak_ptr<const mem_chunk_t> chunk) const {
    impl->assignChunk(std::move(chunk));
}