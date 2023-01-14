#pragma once

#include "GE_pch.h"

#include "Time.h"

namespace GE
{
    class IntervalJob
    {
    public:
        IntervalJob() {}
        IntervalJob(std::function<bool()> func, Time::Miliseconds interval) { Run(func, interval); }
        ~IntervalJob();

        inline bool IsRunning() { return m_running; }

        void Run(std::function<bool()> func, Time::Miliseconds interval);
        void Stop();

    private:
        void RunInternal();

    private:
        std::function<bool()>   m_func;
        Time::Miliseconds       m_interval;
        std::thread             m_thread;
        std::condition_variable m_cv;
        std::mutex              m_mutex;
        bool                    m_running = false;
    };
} // namespace GE