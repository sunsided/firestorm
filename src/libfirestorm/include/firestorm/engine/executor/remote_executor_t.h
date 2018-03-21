//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_REMOTE_EXECUTOR_H
#define PROJECT_REMOTE_EXECUTOR_H

#include <cassert>
#include <firestorm/engine/job/job_t.h>
#include "executor_t.h"

namespace firestorm {

    /// \brief An executor that processes jobs remotely.
    class remote_executor_t final : public executor_t {
    public:
        explicit remote_executor_t(instance_identifier_ptr instance) noexcept
            : executor_t(std::move(instance))
        {
            assert(!instance->local());
        }
        ~remote_executor_t() final = default;

        /// \brief Processes a job on this executor.
        /// \param job The job to process.
        /// \param promise The promise to set the result for.
        void process(const job_t& job, execution_completion_promise promise) noexcept final {
            // TODO: Not implemented
        }
    };

}

#endif //PROJECT_REMOTE_EXECUTOR_H
