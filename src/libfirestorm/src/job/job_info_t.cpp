//
// Created by sunside on 11.03.18.
//

#include <sstream>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <firestorm/synchronization/reader_writer_lock.h>
#include <firestorm/engine/job/job_info_t.h>
#include <firestorm/utils/guid_generator.h>

namespace firestorm {

    job_info_t::job_info_t() noexcept
        : _uuid{guid_generator::get_default().create()}, _created{std::chrono::system_clock::now()}
    {}

    job_info_t::job_info_t(job_info_t&& other) noexcept
        : _uuid{other._uuid}, _created{other._created}
    {}

    std::string job_info_t::id_str() const noexcept {
        std::stringstream ss;
        ss << _uuid;
        return ss.str();
    }

}