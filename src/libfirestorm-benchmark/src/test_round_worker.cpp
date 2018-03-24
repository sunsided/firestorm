//
// Created by sunside on 06.03.18.
//

#include <firestorm/utils/time_conversion.h>
#include <firestorm/logging/logger_t.h>
#include <firestorm/engine/mapreduce/dot_product/keep_all_combiner.h>
#include "test_round.h"

using namespace std;

namespace firestorm {

    void run_test_round_worker(const logger_t& log, const shared_ptr<mapper_factory> &factory,
                               const size_t repetitions, const worker_t &worker,
                               const vector_ptr query,
                               const score_t &expected_best_score,
                               const size_t num_vectors) {
        auto total_duration_ms = static_cast<size_t>(0);
        auto total_num_vectors = static_cast<size_t>(0);

        auto visitor = factory->create();
        keep_all_combiner combiner {};

        for (size_t repetition = 0; repetition < repetitions; ++repetition) {
            auto start_time = chrono::_V2::system_clock::now();

            combiner.begin();
            const auto processed = worker.accept(*visitor, combiner, *query);
            auto results = combiner.finish()->any_cast<vector<score_t>>();

            auto end_time = chrono::_V2::system_clock::now();
            auto local_duration_ms = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

            total_duration_ms += local_duration_ms;
            total_num_vectors += processed;

            // TODO: This should eventually be part of the worker, otherwise we're going through the lists twice.
            score_t best_match{};
            for (const auto &score : results) {
                if (score > best_match || best_match.invalid()) {
                    best_match = score;
                }
            }

            auto local_vectors_per_second =
                    static_cast<float>(num_vectors) / ms_to_s(static_cast<float>(local_duration_ms));
            log->debug("- Round {}/{} matched {} at {}.{} (expected {} at {}.{}); took {} ms for {} vectors ({} vectors/s)",
                       repetition + 1, repetitions,
                       best_match.score(), best_match.index().chunk(), best_match.index().vector_index(),
                       expected_best_score.score(), expected_best_score.index().chunk(), expected_best_score.index().vector_index(),
                       local_duration_ms, processed, local_vectors_per_second);
        }

        auto vectors_per_second =
                static_cast<float>(total_num_vectors) / ms_to_s(static_cast<float>(total_duration_ms));
        log->info("- Processed {} vectors in {} ms ({} vectors/s)",
                  total_num_vectors, total_duration_ms, vectors_per_second);
    }

}
