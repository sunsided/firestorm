//
// Created by sunside on 21.03.18.
//

#include <cassert>
#include <firestorm/engine/executor/local_executor_t.h>

namespace firestorm {

    class local_executor_t::Impl {
    public:
        explicit Impl(instance_identifier_ptr instance, worker_thread_coordinator_ptr wtc) noexcept
            : _instance{std::move(instance)}, _wtc{std::move(wtc)}
        {}

        void process(const job_t& job, job_completion_callback_t callback) noexcept {
            _wtc->process(job, std::move(callback));
        }

    private:
        instance_identifier_ptr _instance;
        worker_thread_coordinator_ptr _wtc;
    };

    local_executor_t::local_executor_t(instance_identifier_ptr instance, worker_thread_coordinator_ptr wtc) noexcept
        : executor_t{instance}, _impl{std::make_unique<local_executor_t::Impl>(instance, std::move(wtc))}
    {
        assert(instance->local());
    }

    local_executor_t::~local_executor_t() = default;

    /// \brief Processes a job on this executor.
    /// \param job The job to process.
    /// \param callback The function to call with the result.
    void local_executor_t::process(const job_t& job, job_completion_callback_t callback) noexcept {
        _impl->process(job, std::move(callback));
    }

}