//
// Created by sunside on 21.02.18.
//

#ifndef PROJECT_LOGGINGEXCEPTION_H
#define PROJECT_LOGGINGEXCEPTION_H

#include <exception>
#include <spdlog/spdlog.h>

namespace firestorm {

    class logging_exception final : public std::runtime_error {
    public:
        explicit logging_exception(const std::string &what)
                : std::runtime_error(what) {}

        ~logging_exception() final = default;
    };

}

#endif //PROJECT_LOGGINGEXCEPTION_H
