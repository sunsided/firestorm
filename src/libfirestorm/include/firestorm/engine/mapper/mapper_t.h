//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_CHUNKMAPPER_H
#define FIRESTORM_CHUNKMAPPER_H

#include <any>
#include <memory>
#include "firestorm/engine/types/mem_chunk_t.h"
#include "firestorm/engine/types/vector_t.h"
#include "firestorm/engine/types/score_t.h"

namespace firestorm {

    /// \brief A stateless class that processes a specified chunk given a specified query vector.
    class mapper_t {
    public:
        virtual ~mapper_t() = default;

        /// \brief Maps a chunk according to the logic of the visitor.
        /// \param chunk The chunk to map.
        /// \param query The query vector to use.
        virtual std::any map(const mem_chunk_t &chunk, const vector_t &query) = 0;
    };

}

#endif //FIRESTORM_CHUNKMAPPER_H
