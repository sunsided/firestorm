//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_EXECUTOR_T_H
#define PROJECT_EXECUTOR_T_H

#include <firestorm/engine/instance_identifier_t.h>

#include <utility>

namespace firestorm {

    class executor_t {
    public:
        explicit executor_t(instance_identifier_ptr instance) noexcept
                : _instance{std::move(instance)}
        {}
        virtual ~executor_t() noexcept = default;

        /// \brief Gets the instance this executor is running on.
        /// \return The instance identifier.
        inline instance_identifier_ptr instance() const noexcept { return _instance; }

        /// \brief Determines whether this instance is the currently running instance.
        /// \return A value indicating whether this instance is local (if true) or remote.
        inline bool local() const {
            return _instance->local();
        };

        // TODO: Add useful processing signature
        virtual void process() = 0;

    private:
        instance_identifier_ptr _instance;
    };

    /// \brief A pointer to an executor instance.
    using executor_ptr = std::shared_ptr<executor_t>;

}

#endif //PROJECT_EXECUTOR_T_H
