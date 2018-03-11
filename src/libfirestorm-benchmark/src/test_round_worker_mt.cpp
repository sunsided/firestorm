//
// Created by sunside on 06.03.18.
//

#include <atomic>
#include <firestorm/utils/time_conversion.h>
#include <firestorm/engine/reducer/keep_all_reducer_factory.h>
#include <firestorm/engine/worker_thread_coordinator.h>
#include "test_round.h"

using namespace std;

namespace firestorm {

    void run_test_round_worker(const shared_ptr<spdlog::logger> &log, const shared_ptr<mapper_factory> &factory,
                               [[ maybe_unused ]] const size_t repetitions,
                               const worker_thread_coordinator& coordinator, const vector_ptr &query,
                               const score_t &expected_best_score,
                               const size_t num_vectors) {

        auto total_duration_ms = static_cast<size_t>(0);
        auto total_num_vectors = static_cast<size_t>(0);

        const auto reducer_factory = make_shared<keep_all_reducer_factory>();
        const auto actual_worker_count = coordinator.effective_worker_count();

        for (size_t repetition = 0; repetition < repetitions; ++repetition) {
            auto start_time = chrono::_V2::system_clock::now();

            const auto info = make_shared<job_info_t>();
            job_t job{info, factory, reducer_factory, query};
            const auto processing_result = coordinator.process(job).get();

            auto end_time = chrono::_V2::system_clock::now();
            auto local_duration_ms = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

            // TODO: This should eventually be part of the worker, otherwise we're going through the lists twice.
            auto results = any_cast<vector<score_t>>(processing_result);
            score_t best_match{};
            for (const auto &score : results) {
                if (score > best_match || best_match.invalid()) {
                    best_match = score;
                }
            }

            total_duration_ms += local_duration_ms;
            total_num_vectors += results.size();

            auto local_vectors_per_second =
                    static_cast<float>(num_vectors) / ms_to_s(static_cast<float>(local_duration_ms));
            log->debug("- Round {}/{} matched {} at {}.{} (expected {} at {}.{}); took {} ms for {} vectors ({} vectors/s)",
                       repetition + 1, repetitions,
                       best_match.score(), best_match.index().chunk(), best_match.index().vector_index(),
                       expected_best_score.score(), expected_best_score.index().chunk(), expected_best_score.index().vector_index(),
                       local_duration_ms, results.size(), local_vectors_per_second);
        }

        // Since N workers run in parallel, the actual duration is 1/Nth the sum of all individual durations.
        auto total_duration_ms_adjusted = static_cast<float>(total_duration_ms);
        auto vectors_per_second = static_cast<float>(total_num_vectors) / ms_to_s(total_duration_ms_adjusted);
        log->info("- Processed {} vectors in {} ms ({} vectors/s, {} workers)",
                  total_num_vectors, total_duration_ms_adjusted, vectors_per_second, actual_worker_count);
    }
}
