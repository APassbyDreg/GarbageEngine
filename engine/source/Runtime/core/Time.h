#pragma once

#include "GE_pch.h"

namespace GE
{
    namespace Time
    {
        using TimeStamp    = std::chrono::system_clock::time_point;
        using DeltaTime    = std::chrono::duration<uint64_t, std::nano>;
        using Seconds      = std::chrono::duration<double>;
        using Miliseconds  = std::chrono::duration<double, std::milli>;
        using Microseconds = std::chrono::duration<double, std::micro>;
        using Nanoseconds  = std::chrono::duration<double, std::nano>;

        inline TimeStamp CurrentTime() { return std::chrono::system_clock::now(); }

        inline double ToSeconds(DeltaTime dt) { return std::chrono::duration_cast<Seconds>(dt).count(); }

        inline double ToMilliseconds(DeltaTime dt) { return std::chrono::duration_cast<Miliseconds>(dt).count(); }

        inline double ToMicroseconds(DeltaTime dt) { return std::chrono::duration_cast<Microseconds>(dt).count(); }

        inline double ToNanoseconds(DeltaTime dt) { return std::chrono::duration_cast<Nanoseconds>(dt).count(); }

        inline std::string ToString(TimeStamp time, const std::string format = "%Y-%m-%d %H:%M:%S")
        {
            std::time_t       tt = std::chrono::system_clock::to_time_t(time);
            std::tm           tm = *std::localtime(&tt); // Locale time-zone, usually UTC by default.
            std::stringstream ss;
            ss << std::put_time(&tm, format.c_str());
            return ss.str();
        }

        inline void Sleep(double seconds) { std::this_thread::sleep_for(std::chrono::duration<double>(seconds)); }

        inline void SleepMilliseconds(double milliseconds)
        {
            std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(milliseconds));
        }
    } // namespace Time
} // namespace GE