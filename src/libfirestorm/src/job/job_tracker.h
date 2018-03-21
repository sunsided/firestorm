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

namespace firestorm {

    /// \brief Keeps track of distributed workloads for a specific job.
    class job_tracker final {
    public:
        explicit job_tracker(const job_t& job) noexcept
            : _info{job.info()}, _promise{}
        {}

        /// \brief Gets the information block of the job.
        /// \return The job information.
        const inline job_info_ptr info() const noexcept { return _info; }

        /// \brief Gets the future that returns the job result.
        /// \return The future result.
        inline std::future<job_status_t> get_future() const noexcept { return _promise.get_future(); }

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
            _promise.set_value(job_status_t::from_failure(FAILURE_UNABLE_TO_START));
            return _promise.get_future();
        }

    private:
        mutable job_info_ptr _info;
        mutable std::promise<job_status_t> _promise;
    };

    using job_tracker_ptr = std::shared_ptr<job_tracker>;

}

#endif //PROJECT_JOB_TRACKER_H
