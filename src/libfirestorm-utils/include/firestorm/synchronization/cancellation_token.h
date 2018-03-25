//
// Created by sunside on 22.03.18.
//

#ifndef PROJECT_CANCELLATION_TOKEN_H
#define PROJECT_CANCELLATION_TOKEN_H

#include <functional>
#include <memory>

namespace firestorm {

    class cancellation_token {
    public:
        /// \brief Function to call back when the token was triggered.
        using callback_t = std::function<void()>;

        /// \brief Type that implements a callback token.
        class cancellation_callback_token_t {
        public:
            virtual ~cancellation_callback_token_t() {};
        };

        /// \brief Token to identify a callback function.
        using callback_token = std::unique_ptr<cancellation_callback_token_t>;

        /// \brief Determines whether this token has been canceled.
        /// \return true if the token has been canceled.
        virtual bool canceled() const noexcept = 0;

        /// \brief Throws an cancellation_exception if this token has been cancelled.
        /// \throws cancellation_exception Thrown when the token was cancelled.
        virtual void throw_if_cancellation_requested() const = 0;

        /// \brief Throws an cancellation_exception if this token has been cancelled.
        /// \param what The message to attach.
        /// \throws cancellation_exception Thrown when the token was cancelled.
        virtual void throw_if_cancellation_requested(const std::string &what) const = 0;

        /// \brief Registers a callback to invoke when the token is canceled.
        /// \param callback The callback function.
        /// \return The token that identifies this callback.
        virtual callback_token register_callback(callback_t callback) = 0;

        /// \brief Unregisters a callback previously added via register_callback.
        /// \param token The token that identifies the callback.
        virtual void unregister_callback(callback_token&& token) = 0;

        /// \brief Unregisters a callback previously added via register_callback.
        /// \param token The token that identifies the callback.
        virtual void unregister_callback(const callback_token& token) = 0;
    };

}

#endif //PROJECT_CANCELLATION_TOKEN_H
