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
        instance_identifier_t() noexcept;
        ~instance_identifier_t() noexcept;

        instance_identifier_t(const instance_identifier_t& others) noexcept = default;
        instance_identifier_t& operator=(const instance_identifier_t& rhs) noexcept;

        /// \brief Determines whether this instance is the currently running instance.
        /// \return A value indicating whether this instance is local (if true) or remote.
        bool local() const noexcept;

        bool operator==(const instance_identifier_t& rhs) const;

        bool operator==(const std::shared_ptr<instance_identifier_t>& rhs) const {
            return *this == *rhs.get();
        }

    private:
        class Impl;
        std::unique_ptr<Impl> _impl;
    };

    using instance_identifier_ptr = std::shared_ptr<instance_identifier_t>;

}

#endif //PROJECT_INSTANCE_IDENTIFIER_H
