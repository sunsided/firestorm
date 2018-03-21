//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_AUTO_RESET_EVENT_H
#define PROJECT_AUTO_RESET_EVENT_H

#include <mutex>
#include <condition_variable>

namespace firestorm {

    class auto_reset_event {
    public:
        explicit auto_reset_event(bool initial = false) noexcept;
        auto_reset_event(const auto_reset_event&) = delete;
        auto_reset_event& operator=(const auto_reset_event&) = delete;

        /// \brief Sets the signal.
        inline void set() noexcept {
            std::lock_guard<std::mutex> lock(_lock);
            _flag = true;
            _signal.notify_all();
        }

        /// \brief Manually resets the signal.
        inline void reset() noexcept {
            std::lock_guard<std::mutex> lock(_lock);
            _flag = false;
        }

        /// \brief Waits for the signal to be set.
        inline void wait() noexcept {
            std::unique_lock<std::mutex> lock(_lock);
            _signal.wait(lock, [this] { return _flag; });
            _flag = false;
        }

        /// \brief Waits for the signal to be set, but not longer than the specified duration.
        /// \param abs_time The time to wait.
        /// \returns true if the signal was set within the specified duration, false otherwise.
        template<class Rep, class Period>
        inline bool wait_for(const std::chrono::duration<Rep, Period>& rel_time) noexcept {
            std::unique_lock<std::mutex> lock(_lock);
            if (!_signal.wait_for(lock, rel_time, [this] { return _flag; })) return false;
            _flag = false;
            return true;
        }

        /// \brief Waits for the signal to be set, but not longer than the specified time.
        /// \param abs_time The time to wait.
        /// \returns true if the signal was set before the specified time, false otherwise.
        template<class Clock, class Duration>
        inline bool wait_until(const std::chrono::time_point<Clock, Duration>& abs_time) noexcept {
            std::unique_lock<std::mutex> lock(_lock);
            if (!_signal.wait_until(lock, abs_time, [this] { return _flag; })) return false;
            _flag = false;
            return true;
        }

    private:
        bool _flag;
        std::mutex _lock;
        std::condition_variable _signal;
    };

}

#endif //PROJECT_AUTO_RESET_EVENT_H
