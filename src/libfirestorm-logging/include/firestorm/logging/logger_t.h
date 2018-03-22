//
// Created by sunside on 22.03.18.
//

#ifndef PROJECT_LOGGER_H
#define PROJECT_LOGGER_H

#include <spdlog/logger.h>

namespace firestorm {

    /// \brief A logger.
    using logger_t = std::shared_ptr<spdlog::logger>;

}

#endif //PROJECT_LOGGER_H
