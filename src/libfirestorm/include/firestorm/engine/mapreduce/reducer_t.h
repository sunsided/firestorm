//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_REDUCER_T_H
#define FIRESTORM_REDUCER_T_H

#include <any>
#include <memory>
#include "reduce_result_t.h"
#include "combine_result_t.h"

namespace firestorm {

    /// \brief Interface to a reducer type.
    class reduce_t {
    public:
        virtual ~reduce_t() = default;

        /// \brief Reduces the results of two combiners.
        /// \param other The result to merge into the local results.
        virtual void reduce(const combine_result& other) = 0;

        /// \brief Reduces the results of two reducers.
        /// \param other The result to merge into the local results.
        virtual void reduce(const reduce_result& other) = 0;
    };

    /// \brief A stateful class that performs mapping result reduction.
    class reducer_t : public virtual reduce_t {
    public:
        ~reducer_t() override = default;

        /// \brief Initializes a reducing operation.
        virtual void begin() = 0;

        /// \brief Finalizes a reducing operation.
        /// \return The reducing result.
        virtual reduce_result finish() = 0;
    };

    /// \brief Pointer to a reducer.
    using reducer_ptr = std::shared_ptr<reducer_t>;

}

#endif //FIRESTORM_REDUCER_T_H
