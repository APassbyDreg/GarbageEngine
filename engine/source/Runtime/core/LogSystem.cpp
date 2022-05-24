#include "LogSystem.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace GE
{
    std::shared_ptr<spdlog::logger>         LogSystem::s_core_logger;
    std::shared_ptr<spdlog::logger>         LogSystem::s_client_logger;
    std::map<std::string, LogSystem::Level> LogSystem::s_string2level = {{"trace", Level::Trace},
                                                                         {"debug", Level::Debug},
                                                                         {"info", Level::Info},
                                                                         {"warn", Level::Warn},
                                                                         {"error", Level::Error},
                                                                         {"critical", Level::Critical}};

    LogSystem::LogSystem()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");

        s_core_logger   = spdlog::stdout_color_mt("GE Core");
        s_client_logger = spdlog::stdout_color_mt("GE App");

        s_core_logger->set_level(spdlog::level::trace);
        s_client_logger->set_level(spdlog::level::trace);
    }

    LogSystem::~LogSystem() {}
} // namespace GE
