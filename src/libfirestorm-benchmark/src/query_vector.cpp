//
// Created by sunside on 06.03.18.
//

#include <random>
#include <firestorm/benchmark/vector_generator.h>
#include "query_vector.h"

using namespace std;

namespace firestorm {
    /// Builds a query vector to test against.
    /// \param NUM_DIMENSIONS The dimensionality of the test vector.
    /// \return
    vector_ptr create_query_vector(const size_t NUM_DIMENSIONS) {
        vector_generator gen {NUM_DIMENSIONS};
        return gen.create_vector(0L);
    }

}