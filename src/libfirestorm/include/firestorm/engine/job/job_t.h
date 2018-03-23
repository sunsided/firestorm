//
// Created by sunside on 10.03.18.
//

#ifndef PROJECT_JOB_T_H
#define PROJECT_JOB_T_H

#include <utility>
#include <firestorm/engine/mapreduce/combiner_factory.h>
#include "firestorm/engine/mapreduce/mapper_factory.h"
#include "firestorm/engine/mapreduce/reducer_factory.h"
#include "job_info_t.h"

namespace firestorm {

    struct job_t : tao::operators::equality_comparable<job_t>,
                   tao::operators::equality_comparable<job_t, job_info_t>,
                   tao::operators::equality_comparable<job_t, job_info_ptr>{
    public:
        job_t(const job_info_ptr &info,
              const mapper_factory_ptr &mf,
              const combiner_factory_ptr &cf,
              const reducer_factory_ptr &rf,
              const vector_ptr &query) noexcept
            : _info{info}, _mf{mf}, _cf{cf}, _rf{rf}, _query{query}
        {}

        job_t(job_t&& other) noexcept
            : _info{std::move(other._info)},
              _mf{std::move(other._mf)},
              _cf{std::move(other._cf)},
              _rf{std::move(other._rf)},
              _query{std::move(other._query)}
        {}

        const inline job_info_ptr info() const noexcept { return _info; }
        const inline mapper_factory_ptr mapper_factory() const noexcept { return _mf; }
        const inline combiner_factory_ptr combiner_factory() const noexcept { return _cf; }
        const inline reducer_factory_ptr reducer_factory() const noexcept { return _rf; }
        const inline vector_ptr query() const noexcept { return _query; }

        bool operator==(const job_t& rhs) const noexcept {
            return *_info == *rhs._info;
        }

        bool operator==(const job_info_t& rhs) const noexcept {
            return *_info == rhs;
        }

        bool operator==(const job_info_ptr& rhs) const noexcept {
            return *_info == *rhs;
        }

    private:
        mutable job_info_ptr _info;
        mutable mapper_factory_ptr _mf;
        mutable combiner_factory_ptr _cf;
        mutable reducer_factory_ptr _rf;
        mutable vector_ptr _query;
    };

}

namespace std {

    template<>
    struct hash<firestorm::job_t> {
        size_t operator()(const firestorm::job_t& k) const {
            return hash<firestorm::job_info_t>()(*k.info());
        }
    };

}

#endif //PROJECT_JOB_T_H
