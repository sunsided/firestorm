//
// Created by sunside on 20.02.18.
//

#include <vector>
#include <logging/LoggerFactory.h>
#include <logging/LoggingException.h>

namespace spd = spdlog;
using namespace std;

class LoggerFactory::Impl {
public:
    Impl() = default;
    ~Impl() = default;

    void add_sink(const spd::sink_ptr& sink) {
        sinks.push_back(sink);
    }

    inline const vector<spd::sink_ptr>& getSinks() const { return sinks; }

private:
    vector<spd::sink_ptr> sinks;
};

LoggerFactory::LoggerFactory()
: impl{make_unique<LoggerFactory::Impl>()} {

}

LoggerFactory::LoggerFactory(LoggerFactory&&) noexcept = default;
LoggerFactory& LoggerFactory::operator=(LoggerFactory&&) noexcept = default;

LoggerFactory::~LoggerFactory() {
    spd::drop_all();
}

LoggerFactory& LoggerFactory::setAsync() {
    try {
        size_t logger_q_size = 8192;
        spd::set_async_mode(logger_q_size);
        return *this;
    }
    catch (...) {
        std::throw_with_nested(LoggingException("Unable to configure asynchronous logging."));
    }
}

LoggerFactory& LoggerFactory::addConsole(const bool color, const spd::level::level_enum logLevel) {
    try {
        auto stdout_sink = color
            ? static_cast<spd::sink_ptr>(make_shared<spd::sinks::ansicolor_stdout_sink_mt>())
            : static_cast<spd::sink_ptr>(make_shared<spdlog::sinks::stdout_sink_mt>());
        stdout_sink->set_level(logLevel);
        impl->add_sink(stdout_sink);
        return *this;
    }
    catch (...) {
        std::throw_with_nested(LoggingException("Unable to register console logging sink."));
    }
}

shared_ptr<spd::logger> LoggerFactory::createLogger(const std::string& logger_name, const spd::level::level_enum logLevel) {
    try {
        auto sinks = impl->getSinks();
        auto logger = make_shared<spd::logger>(logger_name, begin(sinks), end(sinks));
        logger->set_level(logLevel);

        // register by name
        spd::register_logger(logger);

        return logger;
    }
    catch (...) {
        std::throw_with_nested(LoggingException("Unable to create logger \"" + logger_name + "\"."));
    }
}
