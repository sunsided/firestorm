//
// Created by sunside on 21.03.18.
//

#include <firestorm/engine/executor/local_executor_t.h>
#include <firestorm/engine/executor/execution_completion_promise.h>

namespace firestorm {

    class local_executor_t::Impl {
    public:
        explicit Impl(instance_identifier_ptr instance, worker_thread_coordinator_ptr wtc) noexcept
            : _instance{std::move(instance)}, _wtc{std::move(wtc)}
        {}

        /// \brief Processes a job on this executor.
        /// \param job The job to process.
        /// \return The future containing the processing result.
        std::future<execution_result_t> process([[maybe_unused]] const job_t& job) noexcept {
            execution_completion_promise promise{_instance};
            _wtc->process(job, std::move(promise.callback()));
            return promise.get_future();
        }

    private:
        instance_identifier_ptr _instance;
        worker_thread_coordinator_ptr _wtc;
    };

    local_executor_t::local_executor_t(instance_identifier_ptr instance, worker_thread_coordinator_ptr wtc) noexcept
        : executor_t(instance), _impl{std::make_unique<local_executor_t::Impl>(instance, std::move(wtc))}
    {
        assert(instance->local());
    }

    local_executor_t::~local_executor_t() = default;

    /// \brief Processes a job on this executor.
    /// \param job The job to process.
    /// \return The future containing the processing result.
    std::future<execution_result_t> local_executor_t::process(const job_t& job) noexcept {
        return _impl->process(job);
    }

}