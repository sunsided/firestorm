//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_INSTANCE_IDENTIFIER_H
#define PROJECT_INSTANCE_IDENTIFIER_H

#include <memory>

namespace firestorm {

    struct instance_identifier_t {
    public:
        // TODO: Add identifier for remote machines.
        instance_identifier_t() noexcept
            : _local(true)
        {}
        ~instance_identifier_t() noexcept = default;

        instance_identifier_t(const instance_identifier_t& others) noexcept = default;
        instance_identifier_t& operator=(const instance_identifier_t& rhs) noexcept {
            // TODO: Copy ID, ...
            _local = rhs._local;
            return *this;
        }

        /// \brief Determines whether this instance is the currently running instance.
        /// \return A value indicating whether this instance is local (if true) or remote.
        inline bool local() const noexcept { return _local; }

        bool operator==(const instance_identifier_t& rhs) const {
            // TODO: Compare using identifier
            return this == &rhs;
        }

        bool operator==(const std::shared_ptr<instance_identifier_t>& rhs) const {
            // TODO: Compare using identifier
            return this == rhs.get();
        }

    private:
        // TODO: Add GUID, etc
        bool _local;
    };

    using instance_identifier_ptr = std::shared_ptr<instance_identifier_t>;

}

#endif //PROJECT_INSTANCE_IDENTIFIER_H
