//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_REMOTE_EXECUTOR_H
#define PROJECT_REMOTE_EXECUTOR_H

#include <cassert>
#include "executor_t.h"

namespace firestorm {

    /// \brief An executor that processes jobs remotely.
    class remote_executor_t final : public executor_t {
    public:
        explicit remote_executor_t(instance_identifier_ptr instance) noexcept
            : executor_t(std::move(instance))
        {
            assert(!instance->local());
        }
        ~remote_executor_t() final = default;
    };

}

#endif //PROJECT_REMOTE_EXECUTOR_H
