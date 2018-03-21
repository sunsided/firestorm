//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_EXECUTION_STATUS_T_H
#define PROJECT_EXECUTION_STATUS_T_H

#include <utility>

#include <firestorm/engine/instance_identifier_t.h>
#include <firestorm/engine/job/job_result_t.h>

namespace firestorm {

    /// \brief The status of a job execution on a specific instance.
    struct execution_result_t {
    public:
        execution_result_t(instance_identifier_ptr instance, job_result_t result) noexcept
                : _instance(std::move(instance)), _result(result)
        {}

        inline instance_identifier_ptr instance() const noexcept { return _instance; }
        inline job_result_t result() const noexcept { return _result; }

    private:
        instance_identifier_ptr _instance;
        job_result_t _result;
    };

}

#endif //PROJECT_EXECUTION_STATUS_T_H
