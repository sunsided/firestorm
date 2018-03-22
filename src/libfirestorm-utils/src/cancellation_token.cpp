//
// Created by sunside on 22.03.18.
//

#include <atomic>
#include <unordered_set>
#include <firestorm/synchronization/reader_writer_lock.h>
#include <firestorm/synchronization/cancellation_exception.h>
#include <firestorm/synchronization/cancellation_token_source.h>
#include <firestorm/synchronization/cancellation_token.h>

namespace firestorm {

    class cancellation_token::Impl {
    public:
        explicit Impl(bool canceled) noexcept
            : _signal{canceled}, _callbacks{}, _lock{}
        {}

        /// \brief Cancels this token.
        inline void cancel() noexcept {
            const auto was_canceled = _signal.exchange(true);
            if (was_canceled) return;

            const auto lock = _lock.write_lock();
            for (auto& callback : _callbacks) {
                try {
                    callback();
                }
                catch(...) {
                    // TODO: We might want to log this. In any case, we don't care much.
                }
            }

            // Since we are already canceled, we can clear the list now.
            _callbacks.clear();
        }

        /// \brief Determines whether this token has been canceled.
        /// \return true if the token has been canceled.
        inline bool canceled() const noexcept {
            return _signal.load();
        }

        /// \brief Throws an cancellation_exception if this token has been cancelled.
        inline void throw_if_cancellation_requested() const {
            throw_if_cancellation_requested("The operation was cancelled.");
        }

        /// \brief Throws an cancellation_exception if this token has been cancelled.
        /// \param what The message to attach.
        inline void throw_if_cancellation_requested(const std::string &what) const {
            if (!canceled()) return;
            throw cancellation_exception(what);
        }

        /// \brief Registers a callback to invoke when the token is canceled.
        /// \param callback The callback function.
        void register_callback(callback_t& callback) {
            if (canceled()) {
                callback();
                return;
            }

            const auto lock = _lock.write_lock();
            // TODO: Prevent that the same callback is added twice.
            _callbacks.push_back(callback);
        }

    private:
        std::atomic_bool _signal;
        std::vector<callback_t> _callbacks;
        reader_writer_lock _lock;
    };

    cancellation_token::cancellation_token(bool canceled) noexcept
        : _impl{std::make_unique<cancellation_token::Impl>(canceled)}
    {}

    void cancellation_token::cancel() noexcept {
        return _impl->cancel();
    }

    bool cancellation_token::canceled() const noexcept {
        return _impl->canceled();
    }

    void cancellation_token::throw_if_cancellation_requested() const {
        _impl->throw_if_cancellation_requested();
    }

    void cancellation_token::throw_if_cancellation_requested(const std::string &what) const {
        _impl->throw_if_cancellation_requested(what);
    }

    void cancellation_token::register_callback(callback_t& callback) {
        _impl->register_callback(callback);
    }

}