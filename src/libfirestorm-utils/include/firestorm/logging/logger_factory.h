//
// Created by sunside on 20.02.18.
//

#ifndef PROJECT_LOGGERFACTORY_H
#define PROJECT_LOGGERFACTORY_H

#include <memory>
#include <spdlog/spdlog.h>
#include "logger_t.h"

namespace firestorm {

    /// Factory for loggers.
    class logger_factory {
    public:
        logger_factory();
        logger_factory(const logger_factory &other) = delete;
        logger_factory(logger_factory &&other) noexcept;
        ~logger_factory();

        logger_factory &operator=(logger_factory &&other) noexcept;

        /// Sets the logging system to asynchronous mode.
        /// \return A reference to this instance.
        logger_factory &set_async();

        /// Adds a console logger.
        /// \param color If true, enables ANSI color output.
        /// \param log_level The level at which to log.
        /// \return A reference to this instance.
        logger_factory &add_console(spdlog::level::level_enum log_level = spdlog::level::info, bool color = true);

        /// Creates a new logger.
        /// \param logger_name The name of the logger.
        /// \param log_level The level at which to log.
        /// \return The logger.
        logger_t
        create_logger(const std::string& logger_name, spdlog::level::level_enum log_level = spdlog::level::info);

    private:
        class Impl;

        std::unique_ptr<Impl> impl;
    };

}

#endif //PROJECT_LOGGERFACTORY_H
