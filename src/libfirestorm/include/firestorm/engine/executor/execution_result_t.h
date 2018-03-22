//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_EXECUTION_STATUS_T_H
#define PROJECT_EXECUTION_STATUS_T_H

#include <utility>
#include <tao/operators.hpp>

#include <firestorm/engine/instance_identifier_t.h>
#include <firestorm/engine/job/job_result_t.h>

namespace firestorm {

    /// \brief The status of a job execution on a specific instance.
    struct execution_result_t : tao::operators::equality_comparable<execution_result_t> {
    public:
        execution_result_t(instance_identifier_ptr instance, job_result_t result) noexcept
                : _instance(std::move(instance)), _result(std::move(result))
        {}

        inline instance_identifier_ptr instance() const noexcept { return _instance; }
        inline job_result_t result() const noexcept { return _result; }

        inline bool operator==(const execution_result_t& rhs) const {
            return *instance() == *rhs.instance();
        }

    private:
        instance_identifier_ptr _instance;
        job_result_t _result;
    };

}

namespace std {

    template<>
    struct hash<firestorm::execution_result_t> {
        size_t operator()(const firestorm::execution_result_t& k) const
        {
            return hash<firestorm::instance_identifier_ptr>()(k.instance());
        }
    };

}

#endif //PROJECT_EXECUTION_STATUS_T_H
