#pragma once

#include "IntervalJob.h"

#include "Runtime/core/Log/LogSystem.h"

namespace GE
{
    IntervalJob::~IntervalJob() { Stop(); }

    void IntervalJob::Stop()
    {
        if (m_running)
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_running = false;
            }
            m_cv.notify_all();
            m_thread.join();
        }
    }

    void IntervalJob::Run(std::function<bool()> func, Time::Miliseconds interval)
    {
        GE_CORE_ASSERT(m_running == false, "Cannot start in a running interval job");
        m_running  = true;
        m_func     = func;
        m_interval = interval;
        m_thread   = std::thread(&IntervalJob::RunInternal, this);
    }

    void IntervalJob::RunInternal()
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        bool                         exit = false;
        while (!exit)
        {
            exit = m_cv.wait_for(lk, m_interval, [this] { return !m_running; });
            if (!exit)
            {
                exit = m_func();
            }
        }
        m_running = false;
    }
} // namespace GE