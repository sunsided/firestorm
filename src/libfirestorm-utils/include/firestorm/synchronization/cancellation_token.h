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
        typedef std::function<void()> callback_t;

        /// \brief Determines whether this token has been canceled.
        /// \return true if the token has been canceled.
        virtual bool canceled() const noexcept = 0;

        /// \brief Throws an cancellation_exception if this token has been cancelled.
        virtual void throw_if_cancellation_requested() const = 0;

        /// \brief Throws an cancellation_exception if this token has been cancelled.
        /// \param what The message to attach.
        virtual void throw_if_cancellation_requested(const std::string &what) const = 0;

        /// \brief Registers a callback to invoke when the token is canceled.
        /// \param callback The callback function.
        virtual void register_callback(callback_t callback) = 0;
    };

}

#endif //PROJECT_CANCELLATION_TOKEN_H
