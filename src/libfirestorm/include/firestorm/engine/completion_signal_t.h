//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_COMPLETION_CALLBACK_T_H
#define PROJECT_COMPLETION_CALLBACK_T_H

#include <any>
#include <condition_variable>
#include <utility>
#include <atomic>

namespace firestorm {

    /// \brief Defines a signal that indicates job completion.
    ///
    /// Classes inheriting from this should implement operator().
    struct completion_signal_t {
    public:
        completion_signal_t() noexcept
                : _completed{false}, _cond{}
        {}
        virtual ~completion_signal_t() = default;

        completion_signal_t(const completion_signal_t&) = delete;
        completion_signal_t(completion_signal_t&& other) = delete;

        /// \brief Waits for the callback to be triggered.
        /// \param lock The lock to wait on.
        inline void wait(std::unique_lock<std::mutex> lock) {
            _cond.wait(lock, [this] { return completed(); });
        }

        /// \brief Waits for the callback to be triggered, but not longer than the specified duration.
        /// \param lock The lock to wait on.
        /// \param rel_time The time to wait.
        /// \returns true if the callback was triggered in the specified time, false otherwise.
        template<class Rep, class Period>
        inline bool wait_for(std::unique_lock<std::mutex> lock, const std::chrono::duration<Rep, Period>& rel_time) {
            return _cond.wait_for(lock, rel_time, [this] { return completed(); });
        }

        /// \brief Waits for the callback to be triggered, but not longer than the specified time.
        /// \param lock The lock to wait on.
        /// \param abs_time The time to wait.
        /// \returns true if the callback was triggered before the specified time, false otherwise.
        template<class Clock, class Duration>
        inline bool wait_until(std::unique_lock<std::mutex> lock, const std::chrono::time_point<Clock, Duration>& abs_time) {
            return _cond.wait_until(lock, abs_time, [this] { return completed(); });
        }

        /// \brief Gets a value indicating the callback has been called.
        /// \return true if the callback was called.
        inline bool completed() const noexcept { return _completed; }

    protected:

        /// \brief Marks the callback as completed (i.e. called).
        inline void notify_completed() noexcept {
            _completed = true;
            _cond.notify_all();
        }

    private:
        mutable std::atomic_bool _completed;
        mutable std::condition_variable _cond;
    };

}

#endif //PROJECT_COMPLETION_CALLBACK_T_H
