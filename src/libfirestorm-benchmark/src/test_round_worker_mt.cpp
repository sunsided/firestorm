//
// Created by sunside on 06.03.18.
//

#include <atomic>
#include "test_round.h"
#include <firestorm/engine/combiner/keep_all_combiner_factory.h>

using namespace std;

namespace firestorm {

    void run_test_round_worker(const shared_ptr<spdlog::logger> &log, const mapper_factory &factory,
                               const size_t repetitions,
                               const vector<unique_ptr<Worker>> &workers, const vector_t &query,
                               const index_t expected_best_idx,
                               const float expected_best_score,
                               [[maybe_unused]] const size_t num_vectors) {
        atomic<long> total_duration_ms{0L};
        atomic<size_t> total_num_vectors{0L};

        // TODO: For this implementation, have T threads running, blocking on a query queue, answering to a response queue.
        // TODO: Use promises to return results from queue.

        vector<thread> threads;
        vector<shared_ptr<combiner_t>> result_buffers;
        auto actual_worker_count = 0;

        keep_all_combiner_factory combiner_factory{};

        // Start all threads
        for (const auto &t : workers) {
            const auto *worker = t.get();
            if (!worker->has_work()) continue;
            ++actual_worker_count;

            // NOTE: Will be destroyed upon leaving the loop
            auto visitor = factory.create();
            auto combiner = combiner_factory.create();

            auto fun = thread(
                    [&query, &total_duration_ms, &total_num_vectors, repetitions, &log](const Worker *const worker,
                                                                                        std::shared_ptr<mapper_t> visitor,
                                                                                        std::shared_ptr<combiner_t> combiner) {
                        const auto &w = *worker;
                        auto &v = *visitor;
                        auto &c = *combiner;
                        const auto &q = query;

                        for (size_t repetition = 0; repetition < repetitions; ++repetition) {
                            auto start_time = chrono::_V2::system_clock::now();

                            // HACK: We need to clear the results for the repetitions
                            c.begin();
                            const auto processed = w.accept(v, c, q);

                            auto end_time = chrono::_V2::system_clock::now();
                            auto local_duration_ms = chrono::duration_cast<chrono::milliseconds>(
                                    end_time - start_time).count();

                            total_duration_ms += local_duration_ms;
                            total_num_vectors += processed;

                            auto local_vectors_per_second = static_cast<float>(processed) * MS_TO_S / static_cast<float>(local_duration_ms);
                            log->debug("- Round {}/{} took {} ms for {} vectors ({} vectors/s)",
                                       repetition + 1, repetitions, local_duration_ms, processed,
                                       local_vectors_per_second);
                        }
                    }, worker, std::move(visitor), combiner);

            // Set CPU affinity
            // TODO: Explicitly setting CPU affinity might actually hurt performance if the core is loaded otherwise
            // TODO: L2 cache is shared between cores .. chunk sizes might affect performance of other cores?
            // set_thread_affinity(log, t, fun);

            threads.push_back(move(fun));
            result_buffers.push_back(move(combiner));
        }

        // Wait for all threads to join.
        for_each(threads.begin(), threads.end(), [](thread &t) {
            t.join();
        });

        // Aggregate the results
        auto results = any_cast<vector<score_t>>(combiner_factory.combine_all(result_buffers));

        // TODO: This should eventually be part of the worker, otherwise we're going through the lists twice.
        score_t best_match{};
        for (const auto &score : results) {
            if (score > best_match || best_match.invalid()) {
                best_match = score;
            }
        }

        log->debug("- Rounds matched {} at {}.{} (expected {} at {}.{}).",
                   best_match.score(), best_match.index().chunk(), best_match.index().vector_index(),
                   expected_best_score, expected_best_idx.chunk(), expected_best_idx.vector_index());

        // Since N workers run in parallel, the actual duration is 1/Nth the sum of all individual durations.
        auto total_duration_ms_adjusted = static_cast<float>(total_duration_ms) / actual_worker_count;
        auto vectors_per_second = static_cast<float>(total_num_vectors) * MS_TO_S / total_duration_ms_adjusted;
        log->info("- Processed {} vectors in {} ms ({} vectors/s, {} workers)",
                  total_num_vectors, total_duration_ms_adjusted, vectors_per_second, actual_worker_count);
    }

}