//
// Created by sunside on 24.02.18.
//

#include <memory>
#include <random>
#include <spdlog/spdlog.h>
#include <firestorm/vector_t.h>
#include <firestorm/dot_product_naive.h>

using namespace std;

vector_t create_query_vector(const shared_ptr<spdlog::logger> &log, const size_t NUM_DIMENSIONS) {
    const auto seed = 0L;
    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(0.0f, 2.0f);
    auto random = std::bind(distribution, generator);

    // Create a simple query vector
    vector_t query {NUM_DIMENSIONS};
    for (size_t i = 0; i < NUM_DIMENSIONS; ++i) {
        query.data[i] = random();
    }

    vec_normalize_naive(query.data, query.dimensions);
    return query;
}
