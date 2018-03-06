//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_COMBINER_FACTORY_H
#define PROJECT_COMBINER_FACTORY_H

#include <memory>
#include <vector>
#include "firestorm/engine/mapper/mapper_t.h"
#include "combiner_t.h"

namespace firestorm {

    /// \brief Factory that creates combiners.
    class combiner_factory {
    public:
        /// \brief Initializes a new instance of a combiner.
        /// \return The combiner.
        virtual std::shared_ptr<combiner_t> create() const = 0;

        /// \brief Combines a list of items.
        /// \paragraph items The items to combine.
        /// \return The result of the combination.
        std::any combine_all(std::vector<std::shared_ptr<combiner_t>> items) const;
    };

}

#endif //PROJECT_COMBINER_FACTORY_H
