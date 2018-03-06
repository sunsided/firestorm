//
// Created by sunside on 21.02.18.
//

#ifndef PROJECT_LOGGINGEXCEPTION_H
#define PROJECT_LOGGINGEXCEPTION_H

#include <exception>
#include <spdlog/spdlog.h>

class LoggingException final: public std::runtime_error
{
public:
    explicit LoggingException(const std::string& what)
            : std::runtime_error(what)
    {}
    ~LoggingException() final = default;
};

#endif //PROJECT_LOGGINGEXCEPTION_H
