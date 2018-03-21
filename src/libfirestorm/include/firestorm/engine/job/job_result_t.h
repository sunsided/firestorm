//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_JOB_RESULT_T_H
#define PROJECT_JOB_RESULT_T_H

#include <any>
#include "job_status_t.h"

namespace firestorm {

    struct job_result_t {
    public:
        job_result_t(const job_status_t& status, std::any&& result) noexcept
            : _status{status}, _result{result}
        {}

        inline const job_status_t& status() const { return _status; }
        inline const std::any& result() const { return _result; }

    private:
        const job_status_t _status;
        const std::any _result;
    };

}

#endif //PROJECT_JOB_RESULT_T_H
