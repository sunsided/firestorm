//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_WORKERCOORDINATOR_H
#define PROJECT_WORKERCOORDINATOR_H

#include <memory>

namespace firestorm {

    class WorkerCoordinator final {
    public:
        explicit WorkerCoordinator(size_t workerCount) noexcept;

        ~WorkerCoordinator();

        WorkerCoordinator(WorkerCoordinator &&) noexcept;

        WorkerCoordinator &operator=(WorkerCoordinator &&) noexcept;

        /// Registers a memory chunk for processing.
        /// \param chunk The chunk to register.
        void assign_chunk(std::weak_ptr<const mem_chunk_t> chunk) const;

    private:
        class Impl;

        std::unique_ptr<Impl> impl;
    };

}

#endif //PROJECT_WORKERCOORDINATOR_H
