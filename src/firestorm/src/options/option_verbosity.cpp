//
// Created by sunside on 24.02.18.
//

#include <unordered_map>
#include <set>
#include <vector>
#include "options.h"

using namespace std;
namespace spd = spdlog;

static const unordered_map<string, spd::level::level_enum> stringToLevel {
        {"trace", spd::level::trace},
        {"debug", spd::level::debug},
        {"info", spd::level::info},
        {"warn", spd::level::warn},
        {"error", spd::level::critical}};

CLI::Option* add_option(CLI::App &app, const std::string name, spd::level::level_enum &variable, const std::string description, const bool defaulted) {

    unordered_map<spd::level::level_enum, string> levelToString;
    set<string> verbosityNames;

    for(auto kv : stringToLevel) {
        verbosityNames.insert(kv.first);
        levelToString[kv.second] = kv.first;
    }

    CLI::callback_t fun = [&variable](CLI::results_t res) {
        string level;
        bool worked = CLI::detail::lexical_cast(res[0], level);
        if(worked) {
            cerr << level << endl;
            variable = stringToLevel.at(level);
        }
        return worked;
    };

    CLI::Option *opt = app.add_option(name, fun, description, defaulted);
    opt->set_custom_option("LEVEL", 1);
    if(defaulted) {
        std::stringstream out;
        out << levelToString.at(variable);
        opt->set_default_str(out.str());
    }
    return opt;
}