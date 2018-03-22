//
// Created by sunside on 22.03.18.
//

#include <algorithm>
#include <atomic>
#include <vector>
#include <firestorm/synchronization/reader_writer_lock.h>
#include <firestorm/synchronization/cancellation_token.h>
#include <firestorm/synchronization/cancellation_token_source.h>
#include <firestorm/synchronization/cancellation_exception.h>

namespace firestorm {

    class cancellation_token_impl final: public cancellation_token {
    public:
        explicit cancellation_token_impl(bool canceled) noexcept
                : _signal{canceled}, _callbacks{}, _lock{}
        {}
        ~cancellation_token_impl() noexcept = default;

        /// \brief Determines whether this token has been canceled.
        /// \return true if the token has been canceled.
        inline bool canceled() const noexcept final {
            return _signal.load();
        }

        /// \brief Throws an cancellation_exception if this token has been cancelled.
        void throw_if_cancellation_requested() const final {
            throw_if_cancellation_requested("The operation was cancelled.");
        }

        /// \brief Throws an cancellation_exception if this token has been cancelled.
        /// \param what The message to attach.
        void throw_if_cancellation_requested(const std::string &what) const final {
            if (!canceled()) return;
            throw cancellation_exception(what);
        }

        /// \brief Registers a callback to invoke when the token is canceled.
        /// \param callback The callback function.
        void register_callback(callback_t callback) final {
            if (canceled()) {
                callback();
                return;
            }

            const auto lock = _lock.write_lock();
            // TODO: Prevent that the same callback is added twice.
            _callbacks.push_back(callback);
        }

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

    private:
        std::atomic_bool _signal;
        std::vector<callback_t> _callbacks;
        reader_writer_lock _lock;
    };

    class cancellation_token_source::Impl {
    public:
        explicit Impl(bool canceled) noexcept
            : _signal{canceled}, _lock{}
        {}

        /// \brief Cancels all attached tokens.
        inline void cancel() noexcept {
            auto was_canceled = _signal.exchange(true);
            if (was_canceled) return;

            // Cancel all registered tokens.
            {
                const auto lock = _lock.read_lock();
                for (const auto& ptr : _tokens) {
                    auto ct = ptr.lock();
                    if (ct == nullptr) continue;
                    ct->cancel();
                }
            }

            // Delete all expired tokens.
            {
                const auto lock = _lock.write_lock();
                _tokens.erase(
                        std::remove_if(_tokens.begin(), _tokens.end(),
                                       [](const std::weak_ptr<cancellation_token>& ptr) { return ptr.expired(); }),
                        _tokens.end());
            }
        }

        /// \brief Determines whether this source was canceled.
        /// \return true if the source has been canceled.
        inline bool canceled() const noexcept { return _signal.load(); }

        /// \brief Registers a cancellation token.
        /// \param token The cancellation token.
        inline void register_token(const std::shared_ptr<cancellation_token_impl>& token) {
            const auto lock = _lock.write_lock();
            _tokens.push_back(token);
        }

    private:
        std::atomic_bool _signal;
        std::vector<std::weak_ptr<cancellation_token_impl>> _tokens;
        reader_writer_lock _lock;
    };

    cancellation_token_source::cancellation_token_source() noexcept
            : cancellation_token_source(false)
    {}

    cancellation_token_source::cancellation_token_source(bool canceled) noexcept
            : _impl{std::make_unique<cancellation_token_source::Impl>(canceled)}
    {}

    cancellation_token_source::~cancellation_token_source() noexcept = default;

    void cancellation_token_source::cancel() noexcept {
        _impl->cancel();
    }

    bool cancellation_token_source::canceled() const noexcept {
        return _impl->canceled();
    }

    std::shared_ptr<cancellation_token> cancellation_token_source::create_token() {
        auto ct = std::make_shared<cancellation_token_impl>(canceled());
        _impl->register_token(ct);
        if (canceled()) ct->cancel();
        return ct;
    }

}