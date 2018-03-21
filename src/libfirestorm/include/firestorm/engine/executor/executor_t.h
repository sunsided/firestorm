//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_EXECUTOR_T_H
#define PROJECT_EXECUTOR_T_H

#include <utility>
#include <firestorm/engine/instance_identifier_t.h>
#include <firestorm/engine/executor/execution_result_t.h>

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

        /// \brief Processes a job on this executor.
        /// \param job The job to process.
        /// \return The future containing the processing result.
        virtual std::future<execution_result_t> process(const job_t& job) noexcept = 0;

    private:
        instance_identifier_ptr _instance;
    };

    /// \brief A pointer to an executor instance.
    using executor_ptr = std::shared_ptr<executor_t>;

}

#endif //PROJECT_EXECUTOR_T_H
