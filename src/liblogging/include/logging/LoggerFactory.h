//
// Created by sunside on 20.02.18.
//

#ifndef PROJECT_LOGGERFACTORY_H
#define PROJECT_LOGGERFACTORY_H

#include <memory>
#include <spdlog/spdlog.h>

/// Factory for loggers.
class LoggerFactory {
public:
    LoggerFactory();
    LoggerFactory(const LoggerFactory& other) = delete;
    LoggerFactory(LoggerFactory&& other) noexcept;
    ~LoggerFactory();

    LoggerFactory& operator=(LoggerFactory&& other) noexcept;

    /// Sets the logging system to asynchronous mode.
    /// \return A reference to this instance.
    LoggerFactory& setAsync();

    /// Adds a console logger.
    /// \param color If true, enables ANSI color output.
    /// \param logLevel The level at which to log.
    /// \return A reference to this instance.
    LoggerFactory& addConsole(spdlog::level::level_enum logLevel = spdlog::level::info, bool color = true);

    /// Creates a new logger.
    /// \param logger_name The name of the logger.
    /// \param logLevel The level at which to log.
    /// \return The logger.
    std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, spdlog::level::level_enum logLevel = spdlog::level::info);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

#endif //PROJECT_LOGGERFACTORY_H
