//
// Created by sunside on 22.03.18.
//

#ifndef PROJECT_CANCELLATION_EXCEPTION_H
#define PROJECT_CANCELLATION_EXCEPTION_H

#include <stdexcept>

namespace firestorm {

    class cancellation_exception final : public std::runtime_error {
    public:
        explicit cancellation_exception(const std::string &what)
                : std::runtime_error(what) {}

        ~cancellation_exception() final = default;
    };

}

#endif //PROJECT_CANCELLATION_EXCEPTION_H
