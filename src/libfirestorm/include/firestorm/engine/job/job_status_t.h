//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_JOB_STATUS_T_H
#define PROJECT_JOB_STATUS_T_H

namespace firestorm {

    enum job_status_enum {
        STATUS_PENDING = 0,                     ///< The job was created, but not yet scheduled to any executor.
        STATUS_PROCESSING = 1,                  ///< The job was scheduled to at least some executors.
        STATUS_COMPLETED = 2                    ///< The job is completed.
    };

    enum job_completion_enum {
        COMPLETION_NONE = 0,                    ///< The is not completed yet.
        COMPLETION_SUCCEEDED = 1,               ///< The job was completed successfully.
        COMPLETION_SUCCEEDED_PARTIALLY = 2,     ///< Some, but not all of the executors returned results.
        COMPLETION_CANCELLED = 3,               ///< The job was cancelled.
        COMPLETION_FAILED = 4                   ///< The job failed.
    };

    enum job_failure_enum {
        FAILURE_NONE = 0,                       ///< The job did not fail.
        FAILURE_UNABLE_TO_START = 1,            ///< The job was unable to start.
    };

    struct job_status_t {
    public:
        /// \brief Creates a new job status from a failure.
        static job_status_t from_failure(job_failure_enum reason) {
            return job_status_t(STATUS_COMPLETED, COMPLETION_FAILED, reason);
        }

        job_status_t() noexcept
                : job_status_t(STATUS_PENDING, COMPLETION_NONE, FAILURE_NONE)
        {}

        job_status_t(job_status_enum status, job_completion_enum completion, job_failure_enum failure) noexcept
                : _status{status}, _completion{completion}, _failure{failure}
        {}

        inline job_status_enum status() const noexcept { return _status; }
        inline job_completion_enum completion_type() const noexcept { return _completion; }
        inline job_failure_enum failure_type() const noexcept { return _failure; }

    private:
        job_status_enum _status;
        job_completion_enum _completion;
        job_failure_enum _failure;
    };

}

#endif //PROJECT_JOB_STATUS_T_H
