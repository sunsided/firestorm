//
// Created by sunside on 23.03.18.
//

#ifndef PROJECT_COMBINER_T_H
#define PROJECT_COMBINER_T_H

#include <memory>
#include <firestorm/engine/mapreduce/map_result_t.h>
#include "combine_result_t.h"

namespace firestorm {

    /// \brief Interface to a combiner type.
    class combine_t {
    public:
        virtual ~combine_t() = default;

        /// \brief Combines the results of mappers.
        /// \param other The other visitor to merge into the local results.
        virtual void combine(const map_result& other) = 0;
    };

    /// \brief A stateful class that performs mapping result combination.
    class combiner_t : public virtual combine_t {
    public:
        ~combiner_t() override = default;

        /// \brief Initializes a combine operation.
        virtual void begin() = 0;

        /// \brief Finalizes a combine operation.
        /// \return A combine result.
        virtual combine_result finish() = 0;
    };

    /// \brief Pointer to a combiner.
    using combiner_ptr = std::shared_ptr<combiner_t>;

}

#endif //PROJECT_COMBINER_T_H
