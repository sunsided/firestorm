//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_KEEP_ALL_REDUCER_FACTORY_H
#define PROJECT_KEEP_ALL_REDUCER_FACTORY_H

#include "firestorm/engine/mapper/mapper_factory.h"
#include "reducer_factory.h"
#include "firestorm/engine/mapper/dot_product_mapper.h"
#include "keep_all_reducer.h"

namespace firestorm {

    class keep_all_reducer_factory final : public reducer_factory {
    public:
        reducer_ptr create() const final {
            return std::make_shared<keep_all_reducer>();
        }
    };

}

#endif //PROJECT_KEEP_ALL_REDUCER_FACTORY_H
