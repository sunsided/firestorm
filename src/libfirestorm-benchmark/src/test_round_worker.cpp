//
// Created by sunside on 06.03.18.
//

#include "test_round.h"
#include <firestorm/engine/DotProductMapper.h>

using namespace std;

namespace firestorm {

    void run_test_round_worker(const shared_ptr<spdlog::logger> &log, const ChunkMapperFactory &factory,
                               const size_t repetitions, const Worker &worker,
                               const vector_t &query,
                               const size_t expected_best_idx, const float expected_best_score,
                               const size_t num_vectors) {
        auto total_duration_ms = static_cast<size_t>(0);
        auto total_num_vectors = static_cast<size_t>(0);

        // TODO: Should be an interface
        auto visitor = factory.create_mapper();
        auto combiner = factory.create_combiner();

        for (size_t repetition = 0; repetition < repetitions; ++repetition) {
            auto start_time = chrono::_V2::system_clock::now();

            // Keep track of the total sum for validation.
            score_t best_match{};

            combiner->begin();
            const auto processed = worker.accept(*visitor, *combiner, query);
            auto results = any_cast<vector<score_t>>(combiner->finish());

            auto end_time = chrono::_V2::system_clock::now();
            auto local_duration_ms = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

            total_duration_ms += local_duration_ms;
            total_num_vectors += processed;

            // TODO: This should eventually be part of the worker, otherwise we're going through the lists twice.
            for (const auto &score : results) {
                if (score > best_match || best_match.invalid()) {
                    best_match = score;
                }
            }

            auto local_vectors_per_second =
                    static_cast<float>(num_vectors) * MS_TO_S / static_cast<float>(local_duration_ms);
            log->debug("- Round {}/{} matched {} at {} (expected {} at {}); took {} ms for {} vectors ({} vectors/s)",
                       repetition + 1, repetitions,
                       best_match.score(), best_match.vector_idx(),
                       expected_best_score, expected_best_idx,
                       local_duration_ms, processed, local_vectors_per_second);
        }

        auto vectors_per_second =
                static_cast<float>(total_num_vectors) * MS_TO_S / static_cast<float>(total_duration_ms);
        log->info("- Processed {} vectors in {} ms ({} vectors/s)",
                  total_num_vectors, total_duration_ms, vectors_per_second);
    }

}
