//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_CHUNKCOMBINER_H
#define FIRESTORM_CHUNKCOMBINER_H

#include <any>
#include <memory>
#include "firestorm/engine/types/mem_chunk_t.h"
#include "firestorm/engine/types/vector_t.h"
#include "firestorm/engine/types/score_t.h"

namespace firestorm {

    /// A stateful class that performs result combination.
    class combiner_t {
    public:
        virtual ~combiner_t() = default;

        /// Initializes a combining operation.
        virtual void begin() = 0;

        /// Combines the results of two mappers.
        /// \param other The other visitor to merge into the local results.
        /// \return The combined result.
        virtual void combine(std::any&& other) = 0;

        /// Finalizes a combining operation.
        /// \return The combined result.
        virtual std::any finish() = 0;
    };

}

#endif //FIRESTORM_CHUNKCOMBINER_H
