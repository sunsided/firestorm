//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_DOTPRODUCTMAPPERFACTORY_H
#define PROJECT_DOTPRODUCTMAPPERFACTORY_H

#include "ChunkMapperFactory.h"
#include "DotProductMapper.h"

namespace firestorm {

    template<typename Operation>
    class DotProductMapperFactory final : public ChunkMapperFactory {
        static_assert(std::is_convertible<Operation *, dot_product_t *>::value,
                      "Derived type must inherit dot_product_t as public");

    public:
        virtual std::unique_ptr<ChunkMapper> create() const final {
            return std::make_unique<DotProductMapper<Operation>>();
        }
    };

}

#endif //PROJECT_DOTPRODUCTMAPPERFACTORY_H
