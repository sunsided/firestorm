//
// Created by sunside on 21.03.18.
//

#include <cassert>
#include <unordered_set>
#include <firestorm/utils/future_support.h>
#include "job_tracker.h"

namespace firestorm {

    void job_tracker::process_on(executor_ptr executor) noexcept {
        // TODO: Rather than using a promise here, have a dedicated instance that allows for cancellation.
        auto promise = std::make_shared<std::promise<execution_result_t>>();
        auto instance = executor->instance();

        job_completion_callback_t callback {[this, instance, promise](job_result_t&& result) {
            promise->set_value(execution_result_t{instance, result});
            this->_signal->notify(this->info());
        }};

        try {
            _executor_results.insert(std::pair{executor->instance(), promise->get_future().share()});
        }
        catch (...) {
            // TODO: Log the exception.
            promise->set_exception(std::current_exception());
            return;
        }

        executor->process(_job, std::move(callback));
    }

    bool job_tracker::should_update() noexcept {
        // We check all results if the futures have any value.
        for (const auto& pair : _executor_results) {
            if (!is_ready(pair.second)) return false;
        }

        return true;
    }

    bool job_tracker::update(bool force) noexcept {
        if (!_adding_complete && !force) return false;
        if (_already_processed) return true;

        std::unordered_set<execution_result_t> results;
        std::unordered_set<instance_identifier_ptr> unprocessed;

        for (auto& pair : _executor_results) {
            if (is_ready(pair.second)) {
                results.insert(pair.second.get());
                continue;
            }
            if (!force) {
                unprocessed.insert(pair.first);
            }
        }

        auto partial_results = !unprocessed.empty();
        if (partial_results && !force) { // TODO: This might be the last ping. We need to be able to deadline-terminate executors in time!
            return false;
        }

        // TODO: We need to keep track of cancellation here.
        // const auto job_cancelled = false;

        // We need to aggregate results from all executors.
        auto reducer = _job.reducer_factory()->create();
        reducer->begin();

        // At this point, all viable results have been received.
        for (const auto& execution_result : results) {
            const auto& job_result = execution_result.result();
            const auto& js = job_result.status();
            const auto& maybe_result = job_result.result();

            assert(js.status() == job_status::completed);
            assert(js.completion_type() != job_completion::none);

            switch (js.completion_type()) {
                case job_completion::succeeded:
                    assert(maybe_result);
                    break;
                case job_completion::succeeded_partially:
                    partial_results = true;
                    break;
                case job_completion::cancelled:
                    partial_results = true;
                    break;
                case job_completion::failed:
                    partial_results = true;
                    break;
                default:
                    // TODO: Log an error here.
                    assert(false);
            }

            if (!maybe_result) continue;
            reducer->reduce(*maybe_result);
        }

        reduce_result_t result = reducer->finish();

        // TODO: Print report about outcome.

        // If we have partial results, the outcome cannot be successful.
        job_status status = job_status::completed;
        job_completion completion = partial_results
                                    ? job_completion::succeeded_partially
                                    : job_completion::succeeded;
        job_failure failure = job_failure::none;

        job_status_t js {status, completion, failure};
        job_result_t jr {js, std::move(result)};

        _promise.set_value(jr);
        _already_processed = true;

        return true;
    }

}