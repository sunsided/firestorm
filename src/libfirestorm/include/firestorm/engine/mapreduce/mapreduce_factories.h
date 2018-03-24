//
// Created by sunside on 24.03.18.
//

#ifndef PROJECT_MAPREDUCE_FACTORIES_H
#define PROJECT_MAPREDUCE_FACTORIES_H

#include "mapper_factory.h"
#include "combiner_factory.h"
#include "reducer_factory.h"

namespace firestorm {

    class mapreduce_factories final {
    public:
        mapreduce_factories(const mapper_factory_ptr &mf,
                            const combiner_factory_ptr &cf,
                            const reducer_factory_ptr &rf) noexcept
        : _mf{mf}, _cf{cf}, _rf{rf}
        {}

        ~mapreduce_factories() noexcept = default;
        mapreduce_factories(const mapreduce_factories& other) noexcept = default;
        mapreduce_factories(mapreduce_factories&& other) noexcept = default;

        mapreduce_factories& operator=(const mapreduce_factories& other) noexcept = default;
        mapreduce_factories& operator=(mapreduce_factories&& other) noexcept = default;

        inline mapper_factory_ptr mapper_factory() const noexcept {
            return _mf;
        }

        inline combiner_factory_ptr combiner_factory() const noexcept {
            return _cf;
        }

        inline reducer_factory_ptr reducer_factory() const noexcept {
            return _rf;
        }

    private:
        mapper_factory_ptr _mf;
        combiner_factory_ptr _cf;
        reducer_factory_ptr _rf;
    };

}

#endif //PROJECT_MAPREDUCE_FACTORIES_H
