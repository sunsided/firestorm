//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_REDUCER_FACTORY_H
#define PROJECT_REDUCER_FACTORY_H

#include <memory>
#include <vector>
#include "firestorm/engine/mapper/mapper_t.h"
#include "reducer_ptr.h"
#include "reduce_result_t.h"

namespace firestorm {

    /// \brief Factory that creates reducers.
    class reducer_factory {
    public:
        /// \brief Initializes a new instance of a reducer.
        /// \return The combiner.
        virtual reducer_ptr create() const = 0;

        /// \brief Reduces a list of items.
        /// \paragraph items The items to reduce.
        /// \return The result of the reduction.
        reduce_result_t reduce(std::vector<reducer_ptr> items) const;
    };

    /// \brief Pointer to a reducer factory.
    using reducer_factory_ptr = std::shared_ptr<reducer_factory>;

}

#endif //PROJECT_REDUCER_FACTORY_H
