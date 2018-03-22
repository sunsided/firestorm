//
// Created by sunside on 22.03.18.
//

#ifndef PROJECT_CANCELLATION_TOKEN_H
#define PROJECT_CANCELLATION_TOKEN_H

#include <functional>
#include <memory>
#include "cancellation_token_source.h"

namespace firestorm {

    class cancellation_token {
        friend cancellation_token_source;
    public:
        /// \brief Function to call back when the token was triggered.
        typedef std::function<void*> callback_t;

        /// \brief Determines whether this token has been canceled.
        /// \return true if the token has been canceled.
        bool canceled() const noexcept;

        /// \brief Throws an cancellation_exception if this token has been cancelled.
        void throw_if_cancellation_requested() const;

        /// \brief Throws an cancellation_exception if this token has been cancelled.
        /// \param what The message to attach.
        void throw_if_cancellation_requested(const std::string &what) const;

        /// \brief Registers a callback to invoke when the token is canceled.
        /// \param callback The callback function.
        void register_callback(callback_t& callback);

    private:
        explicit cancellation_token(bool canceled) noexcept;

        /// \brief Cancels this token.
        void cancel() noexcept;

    private:
        class Impl;
        std::unique_ptr<Impl> _impl;
    };

}

#endif //PROJECT_CANCELLATION_TOKEN_H
