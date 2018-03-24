//
// Created by sunside on 10.03.18.
//

#ifndef PROJECT_JOB_T_H
#define PROJECT_JOB_T_H

#include <utility>
#include <firestorm/engine/mapreduce/mapreduce_factories.h>
#include "job_info_t.h"

namespace firestorm {

    struct job_t : tao::operators::equality_comparable<job_t>,
                   tao::operators::equality_comparable<job_t, job_info_t>,
                   tao::operators::equality_comparable<job_t, job_info_ptr>{
    public:
        job_t(const job_info_ptr &info,
              const mapreduce_factories &factories,
              const vector_ptr &query) noexcept
            : _info{info}, _f{factories}, _query{query}
        {}

        job_t(job_t&& other) noexcept
            : _info{std::move(other._info)},
              _f{std::move(other._f)},
              _query{std::move(other._query)}
        {}

        const inline job_info_ptr info() const noexcept { return _info; }
        const inline mapper_factory_ptr mapper_factory() const noexcept { return _f.mapper_factory(); }
        const inline combiner_factory_ptr combiner_factory() const noexcept { return _f.combiner_factory(); }
        const inline reducer_factory_ptr reducer_factory() const noexcept { return _f.reducer_factory(); }
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
        mutable mapreduce_factories _f;
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
