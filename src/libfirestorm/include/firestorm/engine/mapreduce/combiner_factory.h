//
// Created by sunside on 23.03.18.
//

#ifndef PROJECT_COMBINER_FACTORY_H
#define PROJECT_COMBINER_FACTORY_H

#include <memory>
#include <vector>
#include "combiner_t.h"

namespace firestorm {

    /// \brief Factory that creates combiners.
    class combiner_factory {
    public:
        /// \brief Initializes a new instance of a combiner.
        /// \return The combiner.
        virtual combiner_ptr create() const = 0;
    };

    /// \brief Pointer to a combiner factory.
    using combiner_factory_ptr = std::shared_ptr<combiner_factory>;

}

#endif //PROJECT_COMBINER_FACTORY_H
