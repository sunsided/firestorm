//
// Created by sunside on 10.03.18.
//

#ifndef PROJECT_JOB_INFO_T_H
#define PROJECT_JOB_INFO_T_H

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <boost/functional/hash.hpp>
#include <tao/operators.hpp>
#include <firestorm/utils/guid.h>

namespace firestorm {

    /// \brief The job information block is used to describe and identify a job.
    struct job_info_t : tao::operators::equality_comparable<job_info_t> {
        job_info_t() noexcept;
        job_info_t(const job_info_t& other) noexcept = default;
        job_info_t(job_info_t&& other) noexcept;

        job_info_t& operator=(const job_info_t& other) noexcept = default;

        /// \brief Gets the ID of this job.
        /// \return The ID.
        inline const guid& id() const noexcept { return _uuid; }

        /// \brief Gets this job's ID as a string representation.
        /// \return The ID.
        std::string id_str() const noexcept;

        /// \brief Gets the creation time of this job.
        /// \return The creation time.
        inline const std::chrono::system_clock::time_point& created() const noexcept {
            return _created;
        }

        bool operator==(const job_info_t& rhs) const noexcept {
            return _uuid == rhs._uuid;
        }

        // TODO: Add deadline (as relative time so different machine clocks work!)

    private:
        guid _uuid;
        std::chrono::system_clock::time_point _created;
    };

    /// \brief Pointer to job information.
    using job_info_ptr = std::shared_ptr<job_info_t>;
}

namespace std {

    template<>
    struct hash<firestorm::job_info_t> {
        inline size_t operator()(const firestorm::job_info_t& k) const {
            boost::hash<boost::uuids::uuid> uuid_hasher;
            return uuid_hasher(k.id());
        }
    };

    template<>
    struct hash<firestorm::job_info_ptr> {
        inline size_t operator()(const firestorm::job_info_ptr& k) const {
            return hash<firestorm::job_info_t>()(*k);
        }
    };

}

#endif //PROJECT_JOB_INFO_T_H
