#pragma once

#include "GE_pch.h"

#include "core/base/Singleton.h"

#include "spdlog/spdlog.h"

namespace GE
{
    class GE_API LogSystem final : public Singleton<LogSystem>
    {
    public:
        enum class Level
        {
            Trace,
            Debug,
            Info,
            Warn,
            Error,
            Critical
        };

    public:
        LogSystem();
        ~LogSystem();

        template<typename... TARGS>
        void LogCore(std::string level, TARGS&&... args)
        {
            Log(s_core_logger, s_string2level[level], std::forward<TARGS>(args)...);
        }

        template<typename... TARGS>
        void LogApplication(std::string level, TARGS&&... args)
        {
            Log(s_client_logger, s_string2level[level], std::forward<TARGS>(args)...);
        }

    private:
        template<typename... TARGS>
        void Log(std::shared_ptr<spdlog::logger> logger, Level level, TARGS&&... args)
        {
            switch (level)
            {
                case Level::Trace:
                    logger->trace(std::forward<TARGS>(args)...);
                    break;
                case Level::Debug:
                    logger->debug(std::forward<TARGS>(args)...);
                    break;
                case Level::Info:
                    logger->info(std::forward<TARGS>(args)...);
                    break;
                case Level::Warn:
                    logger->warn(std::forward<TARGS>(args)...);
                    break;
                case Level::Error:
                    logger->error(std::forward<TARGS>(args)...);
                    break;
                case Level::Critical:
                    logger->critical(std::forward<TARGS>(args)...);
                    criticalCallback(std::forward<TARGS>(args)...);
                    break;
                default:
                    break;
            }
        }

        template<typename... TARGS>
        void criticalCallback(TARGS&&... args)
        {
            const std::string format_str = fmt::format(std::forward<TARGS>(args)...);
            throw std::runtime_error(format_str);
        }

        static std::map<std::string, Level> s_string2level;

        static std::shared_ptr<spdlog::logger> s_core_logger;
        static std::shared_ptr<spdlog::logger> s_client_logger;
    };
} // namespace GE

/* --------------------------- core logging macros -------------------------- */
#define GE_CORE_TRACE(...) ::GE::LogSystem::getInstance().LogCore("trace", __VA_ARGS__)
#define GE_CORE_DEBUG(...) ::GE::LogSystem::getInstance().LogCore("debug", __VA_ARGS__)
#define GE_CORE_INFO(...) ::GE::LogSystem::getInstance().LogCore("info", __VA_ARGS__)
#define GE_CORE_WARN(...) ::GE::LogSystem::getInstance().LogCore("warn", __VA_ARGS__)
#define GE_CORE_ERROR(...) ::GE::LogSystem::getInstance().LogCore("error", __VA_ARGS__)
#define GE_CORE_CRITICAL(...) ::GE::LogSystem::getInstance().LogCore("critical", __VA_ARGS__)

/* -------------------------- client logging macros ------------------------- */
#define GE_APP_TRACE(...) ::GE::LogSystem::getInstance().LogApplication("trace", __VA_ARGS__)
#define GE_APP_DEBUG(...) ::GE::LogSystem::getInstance().LogApplication("debug", __VA_ARGS__)
#define GE_APP_INFO(...) ::GE::LogSystem::getInstance().LogApplication("info", __VA_ARGS__)
#define GE_APP_WARN(...) ::GE::LogSystem::getInstance().LogApplication("warn", __VA_ARGS__)
#define GE_APP_ERROR(...) ::GE::LogSystem::getInstance().LogApplication("error", __VA_ARGS__)
#define GE_APP_CRITICAL(...) ::GE::LogSystem::getInstance().LogApplication("critical", __VA_ARGS__)

/* ----------------------------- assert with log ---------------------------- */
#define GE_CORE_ASSERT(cond, ...) \
    do \
    { \
        if (!(cond)) \
        { \
            GE_CORE_CRITICAL(__VA_ARGS__); \
            __debugbreak(); \
        } \
    } while (false)
#define GE_APP_ASSERT(cond, ...) \
    do \
    { \
        if (!(cond)) \
        { \
            GE_APP_CRITICAL(__VA_ARGS__); \
            __debugbreak(); \
        } \
    } while (false)