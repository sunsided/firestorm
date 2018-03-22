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
#include "job_status_notification.h"

namespace firestorm {

    /// \brief Keeps track of distributed workloads for a specific job.
    class job_tracker final {
    public:
        explicit job_tracker(job_t& job, job_status_notification_ptr signal) noexcept
            : _job{std::move(job)},
              _promise{},
              _future{_promise.get_future().share()},
              _executor_results{},
              _signal{std::move(signal)}
        {}

        ~job_tracker() noexcept {
            notify_upstream();
        }

        /// \brief Gets the information block of the job.
        /// \return The job information.
        const inline job_info_ptr info() const noexcept { return _job.info(); }

        /// \brief Gets the future that returns the job result.
        /// \return The future result.
        inline std::shared_future<job_result_t> get_future() const noexcept { return _future; }

        /// \brief Processes the job on the given executor.
        /// \param job The job to process.
        void process_on(executor_ptr executor) noexcept;

        /// \brief Signals that no more executors will be scheduled.
        inline void complete_adding() noexcept {
            _adding_complete.store(true);
            notify_upstream();
        }

        /// \brief Finalizes the promise with the specified exception.
        /// \param e The exception
        /// \return The future.
        std::future<job_result_t> fail_with_exception(std::exception_ptr& e) noexcept {
            _promise.set_exception(e);
            notify_upstream();
            return _promise.get_future(); // TODO: return _future
        }

        /// \brief Finalizes the promise with a result that indicates the job was unable to start.
        /// \return The future.
        inline std::future<job_result_t> fail_unstarted() noexcept {
            auto js = job_status_t::from_failure(job_failure::unable_to_start);
            job_result_t jr {js};
            _promise.set_value(jr);
            notify_upstream();
            return _promise.get_future(); // TODO: return _future
        }

        /// \brief Determines if the job status can be updated.
        /// \return true if all work items are completed.
        bool should_update() noexcept;

        /// \brief Updates the result of this worker.
        /// \param force Forces updating the result even if not all results are processed.
        /// \return true if the work is completed.
        bool update(bool force) noexcept;

        /// \brief Updates the result of this worker.
        /// \return true if the work is completed.
        bool update() noexcept {
            return update(false);
        }

    protected:

        /// \brief Updates observers about a status change.
        inline void notify_upstream() const noexcept {
            _signal->notify(info());
        }

    private:
        mutable job_t _job;
        mutable std::promise<job_result_t> _promise;
        mutable std::shared_future<job_result_t> _future;
        mutable std::unordered_map<instance_identifier_ptr, std::shared_future<execution_result_t>> _executor_results;
        mutable job_status_notification_ptr _signal;
        mutable std::atomic_bool _adding_complete {false};
        mutable std::atomic_bool _already_processed {false};
    };

    using job_tracker_ptr = std::shared_ptr<job_tracker>;

}

#endif //PROJECT_JOB_TRACKER_H
