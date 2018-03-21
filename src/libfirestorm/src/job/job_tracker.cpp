//
// Created by sunside on 21.03.18.
//

#include <firestorm/utils/future_support.h>
#include "job_tracker.h"

namespace firestorm {

    void job_tracker::process_on(executor_ptr executor) noexcept {
        std::shared_ptr<auto_reset_event> signal = nullptr; // TODO: Use our own signal? Use an upstream thread?

        auto promise = executor->create_promise(signal);
        try {
            // TODO: The future here doesn't help much as we have to query it. We _do_ actually need callbacks here that ping our observation thread.
            _jobs.insert(std::pair{executor->instance(), promise.get_future()});
        }
        catch (...) {
            // auto eptr = std::current_exception();
            // TODO: Log the exception.
            // TODO: Cancel the promise. Do we have to? The value is not used anyway ...
            return;
        }

        executor->process(_job, std::move(promise));
    }

    bool job_tracker::update() noexcept {
        // We check all results if the futures have any value.
        for (const auto& pair : _jobs) {
            if (is_ready(pair.second)) return true;
        }

        return false;
    }

}