//
// Created by sunside on 23.03.18.
//

#ifndef PROJECT_KEEP_ALL_COMBINER_FACTORY_H
#define PROJECT_KEEP_ALL_COMBINER_FACTORY_H

#include "firestorm/engine/mapreduce/mapper_factory.h"
#include "firestorm/engine/mapreduce/mapper/dot_product_mapper.h"
#include "firestorm/engine/mapreduce/combiner_factory.h"
#include "keep_all_combiner.h"

namespace firestorm {

    class keep_all_combiner_factory final : public combiner_factory {
    public:
        keep_all_combiner_factory() noexcept = default;
        virtual ~keep_all_combiner_factory() noexcept = default;

        combiner_ptr create() const final {
            return std::make_shared<keep_all_combiner>();
        }
    };

}

#endif //PROJECT_KEEP_ALL_COMBINER_FACTORY_H
