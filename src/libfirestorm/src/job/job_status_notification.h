//
// Created by sunside on 22.03.18.
//

#ifndef PROJECT_JOB_STATUS_NOTIFICATION_H
#define PROJECT_JOB_STATUS_NOTIFICATION_H

#include <firestorm/synchronization/auto_reset_event.h>
#include <firestorm/engine/job/job_info_t.h>
#include <blockingconcurrentqueue.h>

namespace firestorm {

    using job_signal_queue_t = moodycamel::BlockingConcurrentQueue<job_info_ptr>;
    using job_signal_queue_ptr = std::shared_ptr<job_signal_queue_t>;

    struct job_status_notification {
    public:
        explicit job_status_notification(job_signal_queue_ptr queue) noexcept
            : _queue{std::move(queue)}
        {}

        ~job_status_notification() noexcept = default;

        /// \brief Notifies the owner of the queue about a status change.
        /// \param info The job that is affected by the status change. If nullptr, a general-purpose signal is sent.
        inline void notify(const job_info_ptr& info) const noexcept {
            _queue->enqueue(info);
        }

    private:
        mutable job_signal_queue_ptr _queue;
    };

    using job_status_notification_ptr = std::shared_ptr<job_status_notification>;

}

#endif //PROJECT_JOB_STATUS_NOTIFICATION_H
