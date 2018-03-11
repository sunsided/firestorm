//
// Created by sunside on 10.03.18.
//

#ifndef PROJECT_JOB_INFO_T_H
#define PROJECT_JOB_INFO_T_H

#include <chrono>
#include <memory>

namespace firestorm {

    /// \brief Information about a job to be processed.
    struct job_info_t {
        job_info_t() noexcept;
        job_info_t(const job_info_t& other) noexcept = default;
        job_info_t(job_info_t&& other) noexcept;

        job_info_t& operator=(const job_info_t& other) noexcept = default;

        inline std::chrono::system_clock::time_point created() const {
            return _created;
        }

        // TODO: Add Job ID (GUID or cluster-wide unique ID?)
        // TODO: Add deadline (as relative time so different machine clocks work!)

    private:
        std::chrono::system_clock::time_point _created;
    };

    /// \brief Pointer to job information.
    using job_info_ptr = std::shared_ptr<job_info_t>;

}

#endif //PROJECT_JOB_INFO_T_H
