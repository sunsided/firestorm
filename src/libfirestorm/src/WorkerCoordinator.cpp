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
    explicit Impl(shared_ptr<const ChunkAccessor> accessor) noexcept
        : accessor{std::move(accessor)}
    {
    }

    void addWorker() {
        // "emplace_back might leak if vector cannot extended"
        // https://stackoverflow.com/a/15784982/195651
        workers.push_back(make_unique<Worker>(accessor));
    }

private:
    const shared_ptr<const ChunkAccessor> accessor;
    vector<unique_ptr<Worker>> workers;
};

WorkerCoordinator::WorkerCoordinator(shared_ptr<const ChunkAccessor> accessor, size_t initialCount) noexcept
    : impl{make_unique<Impl>(accessor)}
{
    for (size_t i = 0; i < initialCount; ++i) {
        impl->addWorker();
    }
}

WorkerCoordinator::~WorkerCoordinator() = default;
WorkerCoordinator::WorkerCoordinator(WorkerCoordinator&&) noexcept = default;
WorkerCoordinator& WorkerCoordinator::operator=(WorkerCoordinator&&) noexcept = default;