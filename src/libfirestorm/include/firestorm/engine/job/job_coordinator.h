//
// Created by sunside on 11.03.18.
//

#ifndef PROJECT_JOB_COORDINATOR_H
#define PROJECT_JOB_COORDINATOR_H

#include <memory>
#include <future>
#include <firestorm/engine/types/vector_t.h>
#include <firestorm/engine/reducer/reducer_factory.h>
#include <firestorm/engine/mapper/mapper_factory.h>
#include <firestorm/engine/worker/worker_thread_coordinator.h>
#include "job_status_t.h"

namespace firestorm {

    /// \brief A job coordinator takes care of creating jobs,
    ///        scheduling work and collecting worker results.
    class job_coordinator final {
    public:
        explicit job_coordinator(worker_thread_coordinator_ptr wtc) noexcept;
        ~job_coordinator() noexcept = default;
        job_coordinator(const job_coordinator&) noexcept = delete;
        job_coordinator(job_coordinator&&) noexcept = default;

        std::future<job_status_t> query(const mapper_factory_ptr &mf, const reducer_factory_ptr &rf, const vector_ptr &query) noexcept;

    private:
        class Impl;
        std::unique_ptr<Impl> _impl;
    };

}

#endif //PROJECT_JOB_COORDINATOR_H
