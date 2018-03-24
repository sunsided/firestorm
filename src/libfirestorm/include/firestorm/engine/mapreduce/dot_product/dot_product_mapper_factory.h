//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_DOTPRODUCTMAPPERFACTORY_H
#define PROJECT_DOTPRODUCTMAPPERFACTORY_H

#include "firestorm/engine/mapreduce/mapper_factory.h"
#include "dot_product_mapper.h"

namespace firestorm {

    template<typename Operation>
    class dot_product_mapper_factory final : public mapper_factory {
        static_assert(std::is_convertible<Operation *, dot_product_t *>::value,
                      "Derived type must inherit dot_product_t as public");
    public:
        dot_product_mapper_factory() noexcept = default;
        virtual ~dot_product_mapper_factory() noexcept = default;

        std::shared_ptr<mapper_t> create() const final {
            return std::make_unique<dot_product_mapper<Operation>>();
        }
    };

}

#endif //PROJECT_DOTPRODUCTMAPPERFACTORY_H
