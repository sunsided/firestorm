//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_CHUNKMAPPERFACTORY_H
#define PROJECT_CHUNKMAPPERFACTORY_H

#include <memory>
#include "mapper_t.h"
#include "firestorm/engine/combiner/combiner_t.h"

namespace firestorm {

    class mapper_factory {
    public:
        virtual std::unique_ptr<mapper_t> create() const = 0;
    };

}

#endif //PROJECT_CHUNKMAPPERFACTORY_H
