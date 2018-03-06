//
// Created by sunside on 06.03.18.
//

#include <random>
#include <functional>
#include "query_vector.h"

#include <firestorm/engine/dot_product_naive.h>

using namespace std;

namespace firestorm {

    /// Builds a query vector to test against.
    /// \param NUM_DIMENSIONS The dimensionality of the test vector.
    /// \return
    vector_t create_query_vector(const size_t NUM_DIMENSIONS) {
        const auto seed = 0L;
        default_random_engine generator(seed);
        normal_distribution<float> distribution(0.0f, 2.0f);
        auto random = bind(distribution, generator);

        // Create a simple query vector
        vector_t query{NUM_DIMENSIONS};
        for (size_t i = 0; i < NUM_DIMENSIONS; ++i) {
            query.data[i] = random();
        }

        vec_normalize_naive(query.data, query.dimensions);
        return query;
    }

}