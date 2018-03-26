//
// Created by sunside on 20.02.18.
//

#include <vector>
#include "../include/firestorm/logging/logger_factory.h"
#include "../include/firestorm/logging/logging_exception.h"

namespace spd = spdlog;
using namespace std;

namespace firestorm {

    class logger_factory::Impl {
    public:
        Impl() = default;

        ~Impl() = default;

        void add_sink(const spd::sink_ptr &sink) {
            sinks.push_back(sink);
        }

        inline const vector<spd::sink_ptr> &getSinks() const { return sinks; }

    private:
        vector<spd::sink_ptr> sinks;
    };

    logger_factory::logger_factory()
            : impl{make_unique<logger_factory::Impl>()} {

    }

    logger_factory::logger_factory(logger_factory &&) noexcept = default;

    logger_factory &logger_factory::operator=(logger_factory &&) noexcept = default;

    logger_factory::~logger_factory() {
        spd::drop_all();
    }

    logger_factory &logger_factory::set_async() {
        try {
            size_t logger_q_size = 8192;
            spd::set_async_mode(logger_q_size);
            return *this;
        }
        catch (...) {
            std::throw_with_nested(logging_exception("Unable to configure asynchronous logging."));
        }
    }

    logger_factory &logger_factory::add_console(spdlog::level::level_enum log_level, bool color) {
        try {
            auto stdout_sink = color
                               ? static_cast<spd::sink_ptr>(make_shared<spd::sinks::ansicolor_stdout_sink_mt>())
                               : static_cast<spd::sink_ptr>(make_shared<spdlog::sinks::stdout_sink_mt>());
            stdout_sink->set_level(log_level);
            impl->add_sink(stdout_sink);
            return *this;
        }
        catch (...) {
            std::throw_with_nested(logging_exception("Unable to register console logging sink."));
        }
    }

    logger_t logger_factory::create_logger(const std::string& logger_name, spdlog::level::level_enum log_level) {
        try {
            auto sinks = impl->getSinks();
            auto logger = make_shared<spd::logger>(logger_name, begin(sinks), end(sinks));
            logger->set_level(log_level);
            logger->set_pattern("[%Y-%m-%d %T.%e] [%t] [%n] [%l] %v");

            // register by name
            spd::register_logger(logger);

            return logger;
        }
        catch (...) {
            std::throw_with_nested(logging_exception("Unable to create logger \"" + logger_name + "\"."));
        }
    }

}