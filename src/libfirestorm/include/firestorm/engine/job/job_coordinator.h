//
// Created by sunside on 11.03.18.
//

#ifndef PROJECT_JOB_COORDINATOR_H
#define PROJECT_JOB_COORDINATOR_H

#include <memory>
#include <future>
#include <vector>
#include <firestorm/engine/types/vector_t.h>
#include <firestorm/engine/worker/worker_thread_coordinator.h>
#include <firestorm/engine/executor/executor_t.h>
#include <firestorm/engine/mapreduce/mapreduce_factory.h>
#include "job_status_t.h"

namespace firestorm {

    /// \brief A job coordinator takes care of creating jobs,
    ///        scheduling work and collecting worker results.
    class job_coordinator final {
    public:
        explicit job_coordinator(std::vector<executor_ptr> executors) noexcept;
        ~job_coordinator() noexcept;
        job_coordinator(const job_coordinator&) noexcept = delete;
        job_coordinator(job_coordinator&&) noexcept = default;

        std::shared_future<job_result_t> query(const mapreduce_factory &factories,
                                               const vector_ptr &query) noexcept;

    private:
        class Impl;
        std::unique_ptr<Impl> _impl;
    };

}

#endif //PROJECT_JOB_COORDINATOR_H
