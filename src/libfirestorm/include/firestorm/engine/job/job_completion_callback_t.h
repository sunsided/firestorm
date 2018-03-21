//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_JOB_COMPLETION_CALLBACK_T_H
#define PROJECT_JOB_COMPLETION_CALLBACK_T_H

#include <any>
#include <condition_variable>
#include <utility>
#include <boost/optional.hpp>
#include <firestorm/engine/job/job_result_t.h>

namespace firestorm {

    /// \brief Defines a callback.
    struct job_completion_callback_t final {
    public:
        typedef std::function<void(job_result_t&&)> callback_t;

        explicit job_completion_callback_t(callback_t callback) noexcept
            : _callback{std::move(callback)}
        {}

        ~job_completion_callback_t() = default;
        job_completion_callback_t(const job_completion_callback_t&) = delete;
        job_completion_callback_t(job_completion_callback_t&& other) noexcept
            : _callback{std::move(other._callback)}
        {}

        job_completion_callback_t& operator=(job_completion_callback_t&& other) noexcept {
            _callback = std::move(other._callback);
            return *this;
        }

        inline void operator()(job_result_t&& result) const {
            _callback(std::move(result));
        };

    private:
        callback_t _callback;
    };

}

#endif //PROJECT_JOB_COMPLETION_CALLBACK_T_H
