//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_EXECUTOR_T_H
#define PROJECT_EXECUTOR_T_H

#include <utility>
#include <firestorm/engine/instance_identifier_t.h>
#include <firestorm/engine/executor/execution_result_t.h>
#include <firestorm/engine/job/job_t.h>
#include "execution_completion_promise.h"

namespace firestorm {

    class executor_t {
    public:
        explicit executor_t(instance_identifier_ptr instance) noexcept
                : _instance{std::move(instance)}
        {}
        virtual ~executor_t() noexcept = default;

        /// \brief Gets the instance this executor is running on.
        /// \return The instance identifier.
        inline instance_identifier_ptr instance() const noexcept { return _instance; }

        /// \brief Determines whether this instance is the currently running instance.
        /// \return A value indicating whether this instance is local (if true) or remote.
        inline bool local() const {
            return _instance->local();
        };

        /// \brief Creates a promise that is tied to this executor.
        /// \return The promise.
        inline execution_completion_promise create_promise() const noexcept {
            return execution_completion_promise{_instance};
        }

        /// \brief Creates a promise that is tied to this executor.
        /// \param signal The signal to notify when the executor has finished processing.
        /// \return The promise.
        inline execution_completion_promise create_promise(std::shared_ptr<auto_reset_event> signal) const noexcept {
            return execution_completion_promise{_instance, signal};
        }

        /// \brief Processes a job on this executor.
        /// \param job The job to process.
        /// \return The future containing the processing result.
        inline std::future<execution_result_t> process(const job_t& job) noexcept {
            auto promise = create_promise();
            auto future = promise.get_future();
            process(job, std::move(promise));
            return future;
        }

        /// \brief Processes a job on this executor.
        /// \param job The job to process.
        /// \param promise The promise to set the result for.
        virtual void process(const job_t& job, execution_completion_promise promise) noexcept = 0;

    private:
        instance_identifier_ptr _instance;
    };

    /// \brief A pointer to an executor instance.
    using executor_ptr = std::shared_ptr<executor_t>;

}

#endif //PROJECT_EXECUTOR_T_H
