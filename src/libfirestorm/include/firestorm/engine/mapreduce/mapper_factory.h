//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_CHUNKMAPPERFACTORY_H
#define PROJECT_CHUNKMAPPERFACTORY_H

#include <memory>
#include "mapper_t.h"
#include "reducer_t.h"

namespace firestorm {

    /// \brief Factory that creates mappers.
    class mapper_factory {
    public:
        /// \brief Initializes a new instance of a mapper.
        /// \return The mapper.
        virtual std::shared_ptr<mapper_t> create() const = 0;
    };

    /// \brief Pointer to a mapper factory.
    using mapper_factory_ptr = std::shared_ptr<mapper_factory>;

}

#endif //PROJECT_CHUNKMAPPERFACTORY_H
