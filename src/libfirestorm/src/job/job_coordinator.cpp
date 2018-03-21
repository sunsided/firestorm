//
// Created by sunside on 11.03.18.
//

#include <memory>
#include <utility>
#include <unordered_map>
#include <firestorm/engine/job/job_coordinator.h>
#include <firestorm/engine/job/job_info_t.h>
#include <firestorm/engine/job/job_t.h>
#include <firestorm/engine/executor/executor_t.h>
#include "job_tracker.h"

using namespace std;

namespace firestorm {

    class job_coordinator::Impl {
    public:
        explicit Impl(vector<executor_ptr> executors) noexcept
            :_executors{move(executors)}, _jobs{}
        {}

        ~Impl() noexcept = default;

        future<job_status_t> query(const mapper_factory_ptr &mf, const reducer_factory_ptr &rf,
                          const vector_ptr &query) noexcept;

    private:
        /// \brief Creates an information block for a job.
        /// \return The job information.
        job_info_ptr create_job_info() const;

    private:
        vector<executor_ptr> _executors;
        unordered_map<job_info_ptr, job_tracker_ptr> _jobs;
    };

    job_coordinator::job_coordinator(vector<executor_ptr> executors) noexcept
        : _impl{make_unique<job_coordinator::Impl>(move(executors))}
    {}

    job_info_ptr job_coordinator::Impl::create_job_info() const {
        return make_shared<job_info_t>();
    }

    future<job_status_t> job_coordinator::query(const mapper_factory_ptr &mf, const reducer_factory_ptr &rf,
                                const vector_ptr &query) noexcept {
        return _impl->query(mf, rf, query);
    }

    future<job_status_t> job_coordinator::Impl::query(const mapper_factory_ptr &mf, const reducer_factory_ptr &rf,
                                       const vector_ptr &query) noexcept {

        auto info = create_job_info();
        job_t job { info, mf, rf, query };

        // The tracker has all information required to watch progress and combine results.
        auto tracker = make_shared<job_tracker>(job);
        try {
            const auto insert_result = _jobs.insert({info, tracker});
            if (!insert_result.second) {
                // TODO: The insert failed because of an ID collision. This is highly unlikely and we need to log it.
                return tracker->fail_unstarted();
            }
        }
        catch(...) {
            // The insert failed, e.g. because of a memory error.
            // TODO: Log the exception but return a _failed_ job status.
            //       This would unify the results in that the future never throws because of _us_.
            auto exception = std::current_exception();
            return tracker->fail_with_exception(exception);
        }

        // TODO: Register what a job progress _expects_, so that it can fire a result when all expectations are met.

        // Schedule onto local executors
        // TODO: Rename worker_thread_coordinator to local_executor_t
        // TODO: Schedule onto remote_executors
        // auto result = _wtc->process(job);
        for (const auto& executor : _executors) {
            // TODO: dispatch job to executor
            executor->local();
        }

        // TODO: Currently, worker_thread_coordinator itself contains aggregation logic for a final promise.
        //       However, multiple _machines_ could return these results, so the actual instance concerned
        //       with aggregation should be this coordinator.
        //       Either that, or we treat the result of the worker coordinator as the _same_ we
        //       would expect from a remote machine. In this case, the job coordinator only handles
        //       global output.
        // TODO: Add deadline support.
        return future<job_status_t>{};
    }

}