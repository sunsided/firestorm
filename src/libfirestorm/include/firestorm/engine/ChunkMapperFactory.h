//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_CHUNKMAPPERFACTORY_H
#define PROJECT_CHUNKMAPPERFACTORY_H

#include <memory>
#include "ChunkMapper.h"

namespace firestorm {

    class ChunkMapperFactory {
    public:
        virtual std::unique_ptr<ChunkMapper> create() const = 0;
    };

}

#endif //PROJECT_CHUNKMAPPERFACTORY_H
