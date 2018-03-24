//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_WORKER_RESULT_T_H
#define PROJECT_WORKER_RESULT_T_H

#include <memory>
#include <firestorm/engine/mapreduce/reduce_result_t.h>
#include <firestorm/engine/job/job_info_t.h>

namespace firestorm {

    struct worker_result_t final {
    public:
        explicit worker_result_t(job_info_ptr info, reduce_result& result) noexcept
                : _info{std::move(info)}, _result{std::move(result)}
        {}

        inline job_info_ptr get_info() const { return _info; }
        inline reduce_result get_result() const { return _result; }

    private:
        const job_info_ptr _info;
        const reduce_result _result;
    };

    using worker_result_ptr = std::shared_ptr<worker_result_t>;

}

#endif //PROJECT_WORKER_RESULT_T_H
