//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_WORKERCOORDINATOR_H
#define PROJECT_WORKERCOORDINATOR_H

#include <memory>

class WorkerCoordinator final {
public:
    explicit WorkerCoordinator(size_t initialCount) noexcept;
    ~WorkerCoordinator();

    WorkerCoordinator(WorkerCoordinator&&) noexcept;
    WorkerCoordinator& operator=(WorkerCoordinator&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

#endif //PROJECT_WORKERCOORDINATOR_H
