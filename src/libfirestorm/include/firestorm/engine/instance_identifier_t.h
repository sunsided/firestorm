//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_INSTANCE_IDENTIFIER_H
#define PROJECT_INSTANCE_IDENTIFIER_H

#include <memory>
#include <tao/operators.hpp>
#include <firestorm/utils/guid.h>

namespace firestorm {

    struct instance_identifier_t : tao::operators::equality_comparable<instance_identifier_t>,
                                   tao::operators::equality_comparable<instance_identifier_t, std::shared_ptr<instance_identifier_t>>{
    public:
        // TODO: Add identifier for remote machines.
        instance_identifier_t() noexcept;
        ~instance_identifier_t() noexcept;

        instance_identifier_t(const instance_identifier_t& others) noexcept = default;
        instance_identifier_t& operator=(const instance_identifier_t& rhs) noexcept;

        /// \brief Gets the ID of this job.
        /// \return The ID.
        const guid& id() const noexcept;

        /// \brief Gets this job's ID as a string representation.
        /// \return The ID.
        std::string id_str() const noexcept;

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
