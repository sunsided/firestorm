//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_JOB_TRACKER_H
#define PROJECT_JOB_TRACKER_H

#include <exception>
#include <future>
#include <memory>
#include <utility>
#include <firestorm/engine/job/job_status_t.h>
#include <firestorm/engine/job/job_info_t.h>
#include <firestorm/engine/job/job_t.h>
#include <firestorm/engine/executor/executor_t.h>

namespace firestorm {

    /// \brief Keeps track of distributed workloads for a specific job.
    class job_tracker final {
    public:
        explicit job_tracker(job_t& job) noexcept
            : _job{std::move(job)}, _promise{}, _jobs{}
        {}

        /// \brief Gets the information block of the job.
        /// \return The job information.
        const inline job_info_ptr info() const noexcept { return _job.info(); }

        /// \brief Gets the future that returns the job result.
        /// \return The future result.
        inline std::future<job_status_t> get_future() const noexcept { return _promise.get_future(); }

        /// \brief Processes the job on the given executor.
        /// \param job The job to process.
        void process_on(executor_ptr executor) noexcept;

        /// \brief Finalizes the promise with the specified exception.
        /// \param e The exception
        /// \return The future.
        std::future<job_status_t> fail_with_exception(std::exception_ptr& e) noexcept {
            _promise.set_exception(e);
            return _promise.get_future();
        }

        /// \brief Finalizes the promise with a result that indicates the job was unable to start.
        /// \return The future.
        inline std::future<job_status_t> fail_unstarted() noexcept {
            _promise.set_value(job_status_t::from_failure(job_failure::unable_to_start));
            return _promise.get_future();
        }

        /// \brief Updates the job status.
        /// \return true if the work is completed.
        bool update() noexcept;

    private:
        mutable job_t _job;
        mutable std::promise<job_status_t> _promise;
        mutable std::unordered_map<instance_identifier_ptr, std::future<execution_result_t>> _jobs;
    };

    using job_tracker_ptr = std::shared_ptr<job_tracker>;

}

#endif //PROJECT_JOB_TRACKER_H
