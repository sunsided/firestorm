//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_LOCAL_EXECUTOR_H
#define PROJECT_LOCAL_EXECUTOR_H

#include <cassert>
#include <future>
#include <utility>
#include <firestorm/engine/worker/worker_thread_coordinator.h>
#include <firestorm/engine/job/job_status_t.h>
#include <firestorm/engine/job/job_completion_callback_t.h>
#include "executor_t.h"
#include "execution_completion_promise.h"

namespace firestorm {

    /// \brief An executor that processes jobs locally.
    class local_executor_t final : public executor_t {
    public:
        explicit local_executor_t(instance_identifier_ptr instance, worker_thread_coordinator_ptr wtc) noexcept;
        ~local_executor_t() final;

        /// \brief Processes a job on this executor.
        /// \param job The job to process.
        /// \param callback The function to call with the result.
        void process(const job_t& job, job_completion_callback_t callback) noexcept final;

    private:
        class Impl;
        std::unique_ptr<Impl> _impl;
    };

}

#endif //PROJECT_LOCAL_EXECUTOR_H
