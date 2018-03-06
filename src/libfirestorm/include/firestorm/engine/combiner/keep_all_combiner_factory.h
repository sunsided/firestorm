//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_KEEP_ALL_COMBINER_FACTORY_H
#define PROJECT_KEEP_ALL_COMBINER_FACTORY_H

#include "firestorm/engine/mapper/mapper_factory.h"
#include "combiner_factory.h"
#include "firestorm/engine/mapper/dot_product_mapper.h"
#include "keep_all_combiner.h"

namespace firestorm {

    class keep_all_combiner_factory final : public combiner_factory {
    public:
        std::shared_ptr<combiner_t> create() const final {
            return std::make_shared<keep_all_combiner>();
        }
    };

}

#endif //PROJECT_KEEP_ALL_COMBINER_FACTORY_H
