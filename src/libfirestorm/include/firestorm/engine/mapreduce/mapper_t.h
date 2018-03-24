//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_MAPPER_T_H
#define FIRESTORM_MAPPER_T_H

#include <memory>
#include "firestorm/engine/types/mem_chunk_t.h"
#include "firestorm/engine/types/vector_t.h"
#include "firestorm/engine/types/score_t.h"
#include "map_result_t.h"

namespace firestorm {

    /// \brief A stateless class that processes a specified chunk given a specified query vector.
    class mapper_t {
    public:
        virtual ~mapper_t() = default;

        /// \brief Maps a chunk according to the logic of the visitor.
        /// \param chunk The chunk to map.
        /// \param query The query vector to use.
        virtual map_result map(const mem_chunk_t &chunk, const vector_t &query) const = 0;
    };

    /// \brief Pointer to a mapper.
    using mapper_ptr = std::shared_ptr<mapper_t>;

}

#endif //FIRESTORM_MAPPER_T_H
