//
// Created by sunside on 21.03.18.
//

#include <sstream>
#include <boost/uuid/uuid_io.hpp>
#include <firestorm/engine/instance_identifier_t.h>
#include <firestorm/utils/guid_generator.h>

namespace firestorm {

    class instance_identifier_t::Impl {
    public:
        explicit Impl(bool local, guid id) noexcept
            : _local{local}, _id{id}
        {}

        inline Impl& operator=(const Impl& rhs) noexcept = default;

        inline const guid& id() const { return _id; }

        /// \brief Determines whether this instance is the currently running instance.
        /// \return A value indicating whether this instance is local (if true) or remote.
        inline bool local() const noexcept { return _local; }

        inline bool operator==(const Impl& rhs) const {
            return _id == rhs._id;
        }

    private:
        bool _local;
        guid _id;
    };

    instance_identifier_t::instance_identifier_t() noexcept
        : _impl{std::make_unique<instance_identifier_t::Impl>(true, guid_generator::get_default().create())}
    {}

    instance_identifier_t::~instance_identifier_t() noexcept = default;

    instance_identifier_t& instance_identifier_t::operator=(const instance_identifier_t& rhs) noexcept {
        *_impl = *rhs._impl;
        return *this;
    }

    bool instance_identifier_t::operator==(const instance_identifier_t& rhs) const {
        return _impl == rhs._impl;
    }

    /// \brief Determines whether this instance is the currently running instance.
    /// \return A value indicating whether this instance is local (if true) or remote.
    bool instance_identifier_t::local() const noexcept { return _impl->local(); }

    /// \brief Gets the ID of this job.
    /// \return The ID.
    const guid& instance_identifier_t::id() const noexcept {
        return _impl->id();
    }

    /// \brief Gets this job's ID as a string representation.
    /// \return The ID.
    std::string instance_identifier_t::id_str() const noexcept {
        std::stringstream ss;
        ss << _impl->id();
        return ss.str();
    }
}