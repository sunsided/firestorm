//
// Created by sunside on 11.03.18.
//

#include <memory>
#include <set>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <blockingconcurrentqueue.h>
#include <firestorm/engine/job/job_coordinator.h>
#include <firestorm/synchronization/reader_writer_lock.h>
#include "job_tracker.h"

namespace firestorm {

    class job_coordinator::Impl {
    public:
        explicit Impl(std::vector<executor_ptr> executors) noexcept
            :_executors{std::move(executors)},
             _jobs{},
             _shutdown{false},
             _mutex{},
             _queue{std::make_shared<job_signal_queue_t>()},
             _signal{std::make_shared<job_status_notification>(_queue)},
             _update_thread{&job_coordinator::Impl::update_thread, this}
        {}

        ~Impl() noexcept {
            // We need to signal the update thread it should shut down.
            _shutdown.store(true);
            _signal->notify(nullptr);
            _update_thread.join();
        }

        std::shared_future<job_result_t> query(const mapreduce_factory &factories,
                                               const vector_ptr &query) noexcept;

    private:
        /// \brief Creates an information block for a job.
        /// \return The job information.
        job_info_ptr create_job_info() const;

        /// \brief Thread that updates the progress of each job tracker.
        void update_thread();

    private:
        std::vector<executor_ptr> _executors;
        std::unordered_map<job_info_ptr, job_tracker_ptr> _jobs;
        mutable std::atomic<bool> _shutdown;
        mutable reader_writer_lock _mutex;
        mutable job_signal_queue_ptr _queue;
        mutable job_status_notification_ptr _signal;
        std::thread _update_thread;

        /// \brief The dequeue timeout to use.
        const std::chrono::milliseconds _dequeue_timeout {500};
    };

    job_coordinator::job_coordinator(std::vector<executor_ptr> executors) noexcept
        : _impl{std::make_unique<job_coordinator::Impl>(std::move(executors))}
    {}

    job_coordinator::~job_coordinator() noexcept = default;

    job_info_ptr job_coordinator::Impl::create_job_info() const {
        return std::make_shared<job_info_t>();
    }

    void job_coordinator::Impl::update_thread() {
        const int max_dequeue_count = 16;
        job_info_ptr infos[max_dequeue_count] {nullptr};
        std::unordered_set<job_tracker_ptr> trackers_to_update;

        do {
            trackers_to_update.clear();
            for (auto& info : infos) {
                info = nullptr;
            }

            auto dequeue_count = _queue->wait_dequeue_bulk_timed(infos, max_dequeue_count, _dequeue_timeout);
            if (_shutdown.load()) break;
            if (dequeue_count == 0) continue;

            // Update the job trackers where needed.
            // We lock the job list to avoid invalidation of our
            {
                const auto lock = _mutex.read_lock();
                for (auto i=0U; i<dequeue_count; ++i) {
                    const auto& id = infos[i];
                    try {
                        auto& tracker = _jobs.at(id);
                        if(!tracker->should_update()) continue;
                        trackers_to_update.insert(tracker);
                    }
                    catch(...) {
                        // TODO: Log this exception, then ignore it.
                        // std::current_exception()
                        continue;
                    }
                }
            }

            // We now update the information in the confirmed trackers
            for (const auto& tracker : trackers_to_update) {
                const auto force_update = false; // TODO: Add deadline support
                tracker->update(force_update);
            }
        }
        while(!_shutdown.load());
    }

    std::shared_future<job_result_t> job_coordinator::query(const mapreduce_factory &factories,
                                const vector_ptr &query) noexcept {
        return _impl->query(factories, query);
    }

    std::shared_future<job_result_t> job_coordinator::Impl::query(const mapreduce_factory &factories,
                                       const vector_ptr &query) noexcept {

        auto info = create_job_info();
        job_t job { info, factories, query };

        // The tracker has all information required to watch progress and combine results.
        auto tracker = std::make_shared<job_tracker>(job, _signal);
        try {
            const auto lock = _mutex.write_lock();
            const auto insert_result = _jobs.insert(std::pair{info, tracker});
            if (!insert_result.second) {
                // TODO: The insert failed because of an ID collision. This is highly unlikely and we need to log it.
                return tracker->fail_unstarted();
            }
        }
        catch(...) {
            // The insert failed, e.g. because of a memory error.
            // TODO: Log the exception but return a _failed_ job status. This would unify the results in that the future never throws because of _us_.
            auto exception = std::current_exception();
            return tracker->fail_with_exception(exception);
        }

        // Schedule onto executors
        for (const auto& executor : _executors) {
            tracker->process_on(executor);
        }
        tracker->complete_adding();

        // TODO: Add deadline support.
        return tracker->get_future();
    }

}