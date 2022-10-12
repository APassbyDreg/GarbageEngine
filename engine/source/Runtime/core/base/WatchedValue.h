#pragma once

#include "GE_pch.h"

namespace GE
{

    template<typename T>
    class WatchedValue
    {
        typedef std::function<void(const T, const T)> UpdateCallbackFn;
        static void                                   DefaultCallback(const T val_new, const T val_old) {}

    public:
        WatchedValue(UpdateCallbackFn cb = DefaultCallback) : m_updatedCallback(cb) {}
        WatchedValue(WatchedValue&& v) : m_updatedCallback(v.m_updatedCallback), m_value(v.m_value) {}

        inline T    GetValue() const { return m_value; }
        inline void SetValue(T val)
        {
            T old_val = m_value;
            m_value   = val;
            m_updatedCallback(val, old_val);
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
        T                m_value;
        UpdateCallbackFn m_updatedCallback;
    };
} // namespace GE