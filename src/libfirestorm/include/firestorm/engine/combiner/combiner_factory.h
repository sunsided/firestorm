//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_COMBINER_FACTORY_H
#define PROJECT_COMBINER_FACTORY_H

#include <memory>
#include "firestorm/engine/mapper/mapper_t.h"
#include "combiner_t.h"

namespace firestorm {

    class combiner_factory {
    public:
        virtual std::unique_ptr<combiner_t> create() const = 0;
    };

}

#endif //PROJECT_COMBINER_FACTORY_H
