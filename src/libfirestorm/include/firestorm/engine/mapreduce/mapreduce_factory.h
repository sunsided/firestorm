//
// Created by sunside on 24.03.18.
//

#ifndef PROJECT_MAPREDUCE_FACTORIES_H
#define PROJECT_MAPREDUCE_FACTORIES_H

#include "mapper_factory.h"
#include "combiner_factory.h"
#include "reducer_factory.h"

namespace firestorm {

    class mapreduce_factory final {
    public:
        mapreduce_factory(const mapper_factory_ptr &mf,
                            const combiner_factory_ptr &cf,
                            const reducer_factory_ptr &rf) noexcept
        : _mf{mf}, _cf{cf}, _rf{rf}
        {}

        ~mapreduce_factory() noexcept = default;
        mapreduce_factory(const mapreduce_factory& other) noexcept = default;
        mapreduce_factory(mapreduce_factory&& other) noexcept = default;

        mapreduce_factory& operator=(const mapreduce_factory& other) noexcept = default;
        mapreduce_factory& operator=(mapreduce_factory&& other) noexcept = default;

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
