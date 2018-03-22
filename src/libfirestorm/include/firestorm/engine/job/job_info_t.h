//
// Created by sunside on 10.03.18.
//

#ifndef PROJECT_JOB_INFO_T_H
#define PROJECT_JOB_INFO_T_H

#include <functional>
#include <chrono>
#include <memory>
#include <tao/operators.hpp>

namespace firestorm {

    /// \brief The job information block is used to describe and identify a job.
    struct job_info_t : tao::operators::equality_comparable<job_info_t> {
        job_info_t() noexcept;
        job_info_t(const job_info_t& other) noexcept = default;
        job_info_t(job_info_t&& other) noexcept;

        job_info_t& operator=(const job_info_t& other) noexcept = default;

        inline std::chrono::system_clock::time_point created() const noexcept {
            return _created;
        }

        bool operator==(const job_info_t& rhs) const noexcept {
            return _created == rhs._created;
        }

        // TODO: Add Job ID (GUID or cluster-wide unique ID?)
        // TODO: Adjust hash<job_info_t> for correct unique ID.
        // TODO: Add deadline (as relative time so different machine clocks work!)

    private:
        std::chrono::system_clock::time_point _created;
    };

    /// \brief Pointer to job information.
    using job_info_ptr = std::shared_ptr<job_info_t>;
}

namespace std {

    template<>
    struct hash<firestorm::job_info_t> {
        size_t operator()(const firestorm::job_info_t& k) const
        {
            using namespace std::chrono;

            const auto created = k.created();
            const auto created_ms = time_point_cast<milliseconds>(created);
            const auto created_ms_since_epoch = created_ms.time_since_epoch().count();

            // TODO: Adjust hash<job_info_t> for correct unique ID.
            return hash<long>()(created_ms_since_epoch);
        }
    };

    template<>
    struct hash<firestorm::job_info_ptr> {
        size_t operator()(const firestorm::job_info_ptr& k) const
        {
            return hash<firestorm::job_info_t>()(*k);
        }
    };

}

#endif //PROJECT_JOB_INFO_T_H
