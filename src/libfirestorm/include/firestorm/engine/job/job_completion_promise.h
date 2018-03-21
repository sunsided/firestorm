//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_JOB_COMPLETION_PROMISE_H
#define PROJECT_JOB_COMPLETION_PROMISE_H

#include <any>
#include <future>
#include "job_completion_callback_t.h"
#include "job_result_t.h"

namespace firestorm {

    /// \brief Defines a promise.
    struct job_completion_promise final {
    public:
        explicit job_completion_promise() noexcept
            : _callback{[this](job_result_t&& result) {
                    this->_promise.set_value(result);
                }}
        {}

        ~job_completion_promise() = default;
        job_completion_promise(const job_completion_promise&) = delete;
        job_completion_promise(job_completion_promise&& other) noexcept
                : _promise{std::move(other._promise)}, _callback{std::move(other._callback)}
        {}

        job_completion_promise& operator=(job_completion_promise&& other) noexcept {
            _promise = std::move(other._promise);
            _callback = std::move(other._callback);
            return *this;
        }

        inline std::future<job_result_t> get_future() noexcept { return _promise.get_future(); }

        inline job_completion_callback_t& callback() noexcept {
            return _callback;
        }

    private:
        std::promise<job_result_t> _promise;
        job_completion_callback_t _callback;
    };

}

#endif //PROJECT_JOB_COMPLETION_PROMISE_H
