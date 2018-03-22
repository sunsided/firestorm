//
// Created by sunside on 22.03.18.
//

#include <algorithm>
#include <atomic>
#include <vector>
#include <firestorm/synchronization/reader_writer_lock.h>
#include <firestorm/synchronization/cancellation_token.h>
#include <firestorm/synchronization/cancellation_token_source.h>

namespace firestorm {

    class cancellation_token_source::Impl {
    public:
        explicit Impl(bool canceled, cancellation_token_source* parent) noexcept
            : _signal{canceled}, _lock{}, _parent{parent}
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
                    _parent->cancel_token(ct);
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
        inline void register_token(const std::shared_ptr<cancellation_token>& token) {
            const auto lock = _lock.write_lock();
            _tokens.push_back(token);
        }

    private:
        std::atomic_bool _signal;
        std::vector<std::weak_ptr<cancellation_token>> _tokens;
        reader_writer_lock _lock;
        cancellation_token_source* _parent;
    };

    cancellation_token_source::cancellation_token_source() noexcept
            : cancellation_token_source(false)
    {}

    cancellation_token_source::cancellation_token_source(bool canceled) noexcept
            : _impl{std::make_unique<cancellation_token_source::Impl>(canceled, this)}
    {}

    cancellation_token_source::~cancellation_token_source() noexcept = default;

    void cancellation_token_source::cancel() noexcept {
        _impl->cancel();
    }

    bool cancellation_token_source::canceled() const noexcept {
        return _impl->canceled();
    }

    std::shared_ptr<cancellation_token> cancellation_token_source::create_token() {
        auto ct = std::make_shared<cancellation_token>(canceled());
        _impl->register_token(ct);
        if (canceled()) cancel_token(ct);
        return ct;
    }

    void cancellation_token_source::cancel_token(const std::shared_ptr<cancellation_token>& token) const noexcept {
        token->cancel();
    }

}