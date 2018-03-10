//
// Created by sunside on 10.03.18.
//

#ifndef PROJECT_JOB_T_H
#define PROJECT_JOB_T_H

#include <utility>
#include "mapper/mapper_factory.h"
#include "reducer/reducer_factory.h"
#include "job_info_t.h"

namespace firestorm {

    struct job_t {
    public:
        job_t(const job_info_ptr &info, const mapper_factory_ptr &mf, const reducer_factory_ptr &rf, const vector_ptr &query) noexcept
            : _info{info}, _mf{mf}, _rf{rf}, _query{query}
        {}

        job_t(job_t&& other) noexcept
            : _info{std::move(other._info)}, _mf{std::move(other._mf)}, _rf{std::move(other._rf)}, _query{std::move(other._query)}
        {}

        const inline job_info_ptr info() const { return _info; }
        const inline mapper_factory_ptr mapper_factory() const { return _mf; }
        const inline reducer_factory_ptr reducer_factory() const { return _rf; }
        const inline vector_ptr query() const { return _query; }

    private:
        mutable job_info_ptr _info;
        mutable mapper_factory_ptr _mf;
        mutable reducer_factory_ptr _rf;
        mutable vector_ptr _query;
    };

}

#endif //PROJECT_JOB_T_H
