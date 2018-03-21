//
// Created by sunside on 21.03.18.
//

#include <firestorm/engine/instance_identifier_t.h>

namespace firestorm {

    class instance_identifier_t::Impl {
    public:
        explicit Impl(bool local) noexcept
            : _local(local)
        {}

        inline Impl& operator=(const Impl& rhs) noexcept {
            // TODO: Copy ID, ...
            _local = rhs._local;
            return *this;
        }

        /// \brief Determines whether this instance is the currently running instance.
        /// \return A value indicating whether this instance is local (if true) or remote.
        inline bool local() const noexcept { return _local; }

    private:
        // TODO: Add GUID, etc
        bool _local;
    };

    instance_identifier_t::instance_identifier_t() noexcept
        : _impl{std::make_unique<instance_identifier_t::Impl>(true)}
    {}

    instance_identifier_t::~instance_identifier_t() noexcept = default;

    instance_identifier_t& instance_identifier_t::operator=(const instance_identifier_t& rhs) noexcept {
        *_impl = *rhs._impl;
        return *this;
    }

    bool instance_identifier_t::operator==(const instance_identifier_t& rhs) const {
        // TODO: Compare using identifier
        return this == &rhs;
    }

    /// \brief Determines whether this instance is the currently running instance.
    /// \return A value indicating whether this instance is local (if true) or remote.
    bool instance_identifier_t::local() const noexcept { return _impl->local(); }
}