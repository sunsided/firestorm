//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_REDUCER_T_H
#define FIRESTORM_REDUCER_T_H

#include <iterator>
#include <any>
#include <memory>
#include <firestorm/engine/types/mem_chunk_t.h>
#include <firestorm/engine/types/vector_t.h>
#include <firestorm/engine/types/score_t.h>
#include <firestorm/engine/mapper/map_result_t.h>
#include "reduce_result_t.h"

namespace firestorm {

    /// \brief Interface to a reducer type.
    class reduce_t {
    public:
        virtual ~reduce_t() = default;

        /// \brief Reduces the results of two mappers.
        /// \param other The other visitor to merge into the local results.
        /// \return The reduced result.
        virtual void reduce(const map_result_t& other) = 0;
    };

    /// \brief A stateful class that performs mapping result reduction.
    class reducer_t : public reduce_t {
    public:
        ~reducer_t() override = default;

        /// \brief Initializes a reducing operation.
        virtual void begin() = 0;

        /// \brief Finalizes a reducing operation.
        /// \return The reducing result.
        virtual reduce_result_t finish() = 0;
    };

    /// \brief Pointer to a reducer.
    using reducer_ptr = std::shared_ptr<reducer_t>;

}

#endif //FIRESTORM_REDUCER_T_H
