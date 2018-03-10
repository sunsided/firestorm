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
        job_t(const job_info_ptr &info, const mapper_factory_ptr &mf, const reducer_factory_ptr &rf) noexcept
            : _info{info}, _mf{mf}, _rf{rf}
        {}

        job_t(job_t&& other) noexcept
            : _info{std::move(other._info)}, _mf{std::move(other._mf)}, _rf{std::move(other._rf)}
        {}

        const inline job_info_ptr get_info() const { return _info; }
        const inline mapper_factory_ptr get_mapper_factory() const { return _mf; }
        const inline reducer_factory_ptr get_reducer_factory() const { return _rf; }

    private:
        mutable job_info_ptr _info;
        mutable mapper_factory_ptr _mf;
        mutable reducer_factory_ptr _rf;
    };

}

#endif //PROJECT_JOB_T_H
