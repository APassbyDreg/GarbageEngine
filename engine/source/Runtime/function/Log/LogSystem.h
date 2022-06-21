#pragma once

#include "GE_pch.h"

#include "Runtime/core/base/Singleton.h"
#include "Runtime/core/math/math.h"

#include "spdlog/spdlog.h"

namespace GE
{
    static std::string bytes2string(const unsigned char* bytes, const unsigned int size)
    {
        std::stringstream ss;
        ss << "b[";
        for (int i = 0; i < size; i++)
        {
            ss << std::hex << std::setfill('0') << std::setw(2) << (int)bytes[i];
        }
        ss << "]";
        return ss.str();
    }

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
            throw std::runtime_error("critical error occurs");
        }

        static std::map<std::string, Level> s_string2level;

        static std::shared_ptr<spdlog::logger> s_core_logger;
        static std::shared_ptr<spdlog::logger> s_client_logger;
    };

/* --------------------------- core logging macros -------------------------- */
#define GE_CORE_TRACE(...) ::GE::LogSystem::GetInstance().LogCore("trace", __VA_ARGS__)
#define GE_CORE_DEBUG(...) ::GE::LogSystem::GetInstance().LogCore("debug", __VA_ARGS__)
#define GE_CORE_INFO(...) ::GE::LogSystem::GetInstance().LogCore("info", __VA_ARGS__)
#define GE_CORE_WARN(...) ::GE::LogSystem::GetInstance().LogCore("warn", __VA_ARGS__)
#define GE_CORE_ERROR(...) ::GE::LogSystem::GetInstance().LogCore("error", __VA_ARGS__)
#define GE_CORE_CRITICAL(...) ::GE::LogSystem::GetInstance().LogCore("critical", __VA_ARGS__)

/* -------------------------- client logging macros ------------------------- */
#define GE_APP_TRACE(...) ::GE::LogSystem::GetInstance().LogApplication("trace", __VA_ARGS__)
#define GE_APP_DEBUG(...) ::GE::LogSystem::GetInstance().LogApplication("debug", __VA_ARGS__)
#define GE_APP_INFO(...) ::GE::LogSystem::GetInstance().LogApplication("info", __VA_ARGS__)
#define GE_APP_WARN(...) ::GE::LogSystem::GetInstance().LogApplication("warn", __VA_ARGS__)
#define GE_APP_ERROR(...) ::GE::LogSystem::GetInstance().LogApplication("error", __VA_ARGS__)
#define GE_APP_CRITICAL(...) ::GE::LogSystem::GetInstance().LogApplication("critical", __VA_ARGS__)

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

} // namespace GE