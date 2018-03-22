//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_JOB_RESULT_T_H
#define PROJECT_JOB_RESULT_T_H

#include <any>
#include <boost/optional.hpp>
#include "job_status_t.h"

namespace firestorm {

    struct job_result_t {
    public:
        explicit job_result_t(const job_status_t& status) noexcept
                : _status{status}, _result{}
        {}

        job_result_t(const job_status_t& status, std::any&& result) noexcept
            : _status{status}, _result{result}
        {}

        inline const job_status_t& status() const { return _status; }
        inline const boost::optional<std::any>& result() const { return _result; }

    private:
        const job_status_t _status;
        const boost::optional<std::any> _result;
    };

}

#endif //PROJECT_JOB_RESULT_T_H
