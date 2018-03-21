//
// Created by sunside on 10.03.18.
//

#ifndef PROJECT_JOB_INFO_T_H
#define PROJECT_JOB_INFO_T_H

#include <functional>
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

        bool operator==(const job_info_t& rhs) {
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
        typedef firestorm::job_info_t argument_type;
        typedef size_t result_type;

        result_type operator()(const argument_type& k) const
        {
            using namespace std::chrono;

            const auto created = k.created();
            const auto created_ms = time_point_cast<milliseconds>(created);
            const auto created_ms_since_epoch = created_ms.time_since_epoch().count();

            // TODO: Adjust hash<job_info_t> for correct unique ID.
            return hash<long>()(created_ms_since_epoch);
        }
    };

}

#endif //PROJECT_JOB_INFO_T_H
