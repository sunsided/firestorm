//
// Created by sunside on 11.03.18.
//

#include <firestorm/engine/job/job_info_t.h>

namespace firestorm {

    job_info_t::job_info_t() noexcept
            : _created{std::chrono::system_clock::now()}
    {}

    job_info_t::job_info_t(job_info_t&& other) noexcept
        : _created{other._created}
    {}

}