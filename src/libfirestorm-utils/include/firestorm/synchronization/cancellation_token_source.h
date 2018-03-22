//
// Created by sunside on 22.03.18.
//

#ifndef PROJECT_CANCELLATION_TOKEN_SOURCE_H
#define PROJECT_CANCELLATION_TOKEN_SOURCE_H

#include <chrono>
#include <memory>
#include <thread>
#include "cancellation_token.h"

namespace firestorm {

    class cancellation_token_source final {
    public:
        cancellation_token_source() noexcept;
        explicit cancellation_token_source(bool canceled) noexcept;
        ~cancellation_token_source() noexcept;

        template<typename _Rep, typename _Period>
        explicit cancellation_token_source(const std::chrono::duration<_Rep, _Period>& rel_time) noexcept
            : cancellation_token_source()
        {
            std::thread([this, rel_time]() {
                std::this_thread::sleep_for(rel_time);
                cancel();
            }).detach();
        }

        template<typename _Clock, typename _Duration>
        explicit cancellation_token_source(const std::chrono::time_point<_Clock, _Duration>& abs_time) noexcept
            : cancellation_token_source()
        {
            std::thread([this, abs_time]() {
                std::this_thread::sleep_until(abs_time);
                cancel();
            }).detach();
        }

        cancellation_token_source(cancellation_token_source&&) = default;
        cancellation_token_source& operator=(cancellation_token_source&&) = default;

        /// \brief Cancels all attached tokens.
        void cancel() noexcept;

        /// \brief Determines whether this source was canceled.
        /// \return true if the source has been canceled.
        bool canceled() const noexcept;

        /// \brief Creates a cancellation token.
        /// \returns The cancellation token.
        std::shared_ptr<cancellation_token> create_token();

    private:
        class Impl;
        std::unique_ptr<Impl> _impl;
    };

}

#endif //PROJECT_CANCELLATION_TOKEN_SOURCE_H
