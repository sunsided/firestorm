//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_JOB_STATUS_T_H
#define PROJECT_JOB_STATUS_T_H

namespace firestorm {

    enum class job_status {
        pending = 0,                     ///< The job was created, but not yet scheduled to any executor.
        processing = 1,                  ///< The job was scheduled to at least some executors.
        completed = 2                    ///< The job is completed.
    };

    enum class job_completion {
        none = 0,                    ///< The is not completed yet.
        succeeded = 1,               ///< The job was completed successfully.
        succeeded_partially = 2,     ///< Some, but not all of the executors returned results.
        cancelled = 3,               ///< The job was cancelled.
        failed = 4                   ///< The job failed.
    };

    enum class job_failure {
        none = 0,                       ///< The job did not fail.
        unable_to_start = 1,            ///< The job was unable to start.
    };

    struct job_status_t {
    public:
        /// \brief Creates a new job status from a failure.
        static job_status_t from_failure(job_failure reason) {
            return job_status_t(job_status::completed, job_completion::failed, reason);
        }

        job_status_t() noexcept
                : job_status_t(job_status::pending, job_completion::none, job_failure::none)
        {}

        job_status_t(job_status status, job_completion completion, job_failure failure) noexcept
                : _status{status}, _completion{completion}, _failure{failure}
        {}

        job_status_t(const job_status_t&) noexcept = default;
        job_status_t(job_status_t&&) noexcept = default;

        inline job_status status() const noexcept { return _status; }
        inline job_completion completion_type() const noexcept { return _completion; }
        inline job_failure failure_type() const noexcept { return _failure; }

    private:
        job_status _status;
        job_completion _completion;
        job_failure _failure;
    };

}

#endif //PROJECT_JOB_STATUS_T_H
