//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_LOCAL_EXECUTOR_H
#define PROJECT_LOCAL_EXECUTOR_H

#include <cassert>
#include <utility>
#include <firestorm/engine/worker/worker_thread_coordinator.h>
#include "executor_t.h"

namespace firestorm {

    /// \brief An executor that processes jobs locally.
    class local_executor_t final : public executor_t {
    public:
        explicit local_executor_t(instance_identifier_ptr instance, worker_thread_coordinator_ptr wtc) noexcept
            : _wtc{std::move(wtc)}, executor_t(std::move(instance))
        {
            assert(instance->local());
        }
        ~local_executor_t() final = default;

    private:
        worker_thread_coordinator_ptr _wtc;
    };

}

#endif //PROJECT_LOCAL_EXECUTOR_H
