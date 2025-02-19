#pragma once

#include "GE_pch.h"

#include "Runtime/core/Base/Singleton.h"
#include "Runtime/core/Math/Math.h"

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

    
    enum class LogLevel
    {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Critical
    };

    class GE_API LogSystem final : public Singleton<LogSystem, -1001>
    {
    public:
        LogSystem();
        ~LogSystem();

        template<typename... TArgs>
        void LogCore(LogLevel level, TArgs&&... args)
        {
            Log(s_core_logger, level, std::forward<TArgs>(args)...);
        }

        template<typename... TArgs>
        void LogApplication(LogLevel level, TArgs&&... args)
        {
            Log(s_client_logger, level, std::forward<TArgs>(args)...);
        }

    private:
        template<typename... TArgs>
        void Log(std::shared_ptr<spdlog::logger> logger, LogLevel level, TArgs&&... args)
        {
            switch (level)
            {
                case LogLevel::Trace:
                    logger->trace(std::forward<TArgs>(args)...);
                    break;
                case LogLevel::Debug:
                    logger->debug(std::forward<TArgs>(args)...);
                    break;
                case LogLevel::Info:
                    logger->info(std::forward<TArgs>(args)...);
                    break;
                case LogLevel::Warn:
                    logger->warn(std::forward<TArgs>(args)...);
                    break;
                case LogLevel::Error:
                    logger->error(std::forward<TArgs>(args)...);
                    break;
                case LogLevel::Critical:
                    logger->critical(std::forward<TArgs>(args)...);
                    criticalCallback(std::forward<TArgs>(args)...);
                    break;
                default:
                    break;
            }
        }

        template<typename... TArgs>
        void criticalCallback(TArgs&&... args)
        {
            throw std::runtime_error("critical error occurs");
        }

        static std::map<std::string, LogLevel> s_string2level;

        static std::shared_ptr<spdlog::logger> s_core_logger;
        static std::shared_ptr<spdlog::logger> s_client_logger;
    };

/* --------------------------- core logging macros -------------------------- */
#define GE_CORE_TRACE(...) ::GE::LogSystem::GetInstance().LogCore(LogLevel::Trace, " "+std::format(__VA_ARGS__))
#define GE_CORE_DEBUG(...) ::GE::LogSystem::GetInstance().LogCore(LogLevel::Debug, " "+std::format(__VA_ARGS__))
#define GE_CORE_INFO(...) ::GE::LogSystem::GetInstance().LogCore(LogLevel::Info, " "+std::format(__VA_ARGS__))
#define GE_CORE_WARN(...) ::GE::LogSystem::GetInstance().LogCore(LogLevel::Warn, " "+std::format(__VA_ARGS__))
#define GE_CORE_ERROR(...) ::GE::LogSystem::GetInstance().LogCore(LogLevel::Error, " "+std::format(__VA_ARGS__))
#define GE_CORE_CRITICAL(...) ::GE::LogSystem::GetInstance().LogCore(LogLevel::Critical, " "+std::format(__VA_ARGS__))

/* -------------------------- client logging macros ------------------------- */
#define GE_APP_TRACE(...) ::GE::LogSystem::GetInstance().LogApplication(LogLevel::Trace, " "+std::format(__VA_ARGS__))
#define GE_APP_DEBUG(...) ::GE::LogSystem::GetInstance().LogApplication(LogLevel::Debug, " "+std::format(__VA_ARGS__))
#define GE_APP_INFO(...) ::GE::LogSystem::GetInstance().LogApplication(LogLevel::Info, " "+std::format(__VA_ARGS__))
#define GE_APP_WARN(...) ::GE::LogSystem::GetInstance().LogApplication(LogLevel::Warn, " "+std::format(__VA_ARGS__))
#define GE_APP_ERROR(...) ::GE::LogSystem::GetInstance().LogApplication(LogLevel::Error, " "+std::format(__VA_ARGS__))
#define GE_APP_CRITICAL(...) ::GE::LogSystem::GetInstance().LogApplication(LogLevel::Critical, " "+std::format(__VA_ARGS__))

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

#define GE_CORE_CHECK(cond, ...) \
    do \
    { \
        if (!(cond)) \
        { \
            GE_CORE_ERROR(__VA_ARGS__); \
        } \
    } while (false)

#define GE_APP_CHECK(cond, ...) \
    do \
    { \
        if (!(cond)) \
        { \
            GE_APP_ERROR(__VA_ARGS__); \
        } \
    } while (false)

} // namespace GE