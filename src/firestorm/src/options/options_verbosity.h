//
// Created by sunside on 24.02.18.
//

#ifndef PROJECT_OPTIONS_VERBOSITY_H
#define PROJECT_OPTIONS_VERBOSITY_H

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

CLI::Option* add_option(CLI::App &app, std::string name, spdlog::level::level_enum &variable, std::string description = "", bool defaulted = false);

#endif //PROJECT_OPTIONS_VERBOSITY_H
