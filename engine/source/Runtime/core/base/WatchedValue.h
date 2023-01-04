#pragma once

#include "GE_pch.h"

namespace GE
{

    template<typename T>
    class WatchedValue
    {
        using UpdateCallbackFn = std::function<void()>; // old values are get by GetLastValue()

    public:
        WatchedValue() {}
        WatchedValue(WatchedValue&& v) :
            m_updatedCallbacks(v.m_updatedCallbacks), m_value(v.m_value), m_lastValue(v.m_lastValue)
        {}

        inline void AddCallback(UpdateCallbackFn cb) { m_updatedCallbacks.push_back(cb); }
        inline T    GetValue() const { return m_value; }
        inline T    GetLastValue() const { return m_lastValue; }
        inline void SetValue(T val)
        {
            m_lastValue = m_value;
            m_value     = val;
            for (auto&& cb : m_updatedCallbacks)
                cb();
        }

        /* --------------------------- convertions -------------------------- */

        inline WatchedValue& operator=(T rhs)
        {
            SetValue(rhs);
            return *this;
        }
        inline WatchedValue& operator=(WatchedValue&& rhs)
        {
            SetValue(rhs.m_value);
            return *this;
        }
        operator T() const { return GetValue(); }

    private:
        T                             m_value, m_lastValue;
        std::vector<UpdateCallbackFn> m_updatedCallbacks;
    };
} // namespace GE