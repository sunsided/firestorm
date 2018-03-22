//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_EXECUTION_COMPLETION_PROMISE_H
#define PROJECT_EXECUTION_COMPLETION_PROMISE_H

#include <any>
#include <future>
#include <utility>
#include <boost/optional.hpp>
#include <firestorm/engine/job/job_completion_callback_t.h>
#include <firestorm/engine/instance_identifier_t.h>
#include <firestorm/engine/job/job_status_t.h>
#include <firestorm/engine/auto_reset_event.h>
#include "execution_result_t.h"

namespace firestorm {

    /// \brief Defines a promise.
    struct execution_completion_promise final {
    public:
        explicit execution_completion_promise(instance_identifier_ptr instance) noexcept
            : execution_completion_promise(instance, nullptr)
        {}

        execution_completion_promise(instance_identifier_ptr instance, std::shared_ptr<auto_reset_event> signal) noexcept
                : _instance{std::move(instance)},
                  _promise{},
                  _signal{signal},
                  _callback{[this](job_result_t&& result) {
                      execution_result_t exec_status{this->_instance, result};
                      this->_promise.set_value(exec_status);
                      if (this->_signal != nullptr) {
                          this->_signal->set();
                      }
                    }}
        {}

        ~execution_completion_promise() = default;
        execution_completion_promise(const execution_completion_promise&) = delete;
        execution_completion_promise(execution_completion_promise&& other) noexcept
                : _instance{std::move(other._instance)},
                  _promise{std::move(other._promise)},
                  _signal{std::move(other._signal)},
                  _callback{std::move(other._callback)}
        {}

        execution_completion_promise& operator=(execution_completion_promise&& other) noexcept {
            _instance = std::move(other._instance);
            _promise = std::move(other._promise);
            _signal = std::move(other._signal);
            _callback = std::move(other._callback);
            return *this;
        }

        inline instance_identifier_ptr instance() noexcept { return _instance; }

        inline std::future<execution_result_t> get_future() noexcept { return _promise.get_future(); }

        inline job_completion_callback_t& callback() noexcept {
            return _callback;
        }

    private:
        instance_identifier_ptr _instance;
        std::promise<execution_result_t> _promise;
        std::shared_ptr<auto_reset_event> _signal;
        job_completion_callback_t _callback;
    };

}

#endif //PROJECT_EXECUTION_COMPLETION_PROMISE_H
