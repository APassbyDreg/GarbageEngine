#pragma once

#include "GE_pch.h"

#include "core/math/math.h"

#include "../MsgTypeBase.h"

namespace GE
{
    class GE_API WindowCloseMsg : public MsgBodyBase
    {
    public:
        GE_MSG_BODY_COMMON_IMPL(WindowCloseMsg)
    };

    class GE_API WindowResizeMsg : public MsgBodyBase
    {
    public:
        GE_MSG_BODY_COMMON_IMPL(WindowResizeMsg)

        WindowResizeMsg(uint width, uint height) : m_width(width), m_height(height) {}

        int2 getSize() const { return int2(m_width, m_height); }

    private:
        uint m_width;
        uint m_height;
    };

    class GE_API WindowFocusMsg : public MsgBodyBase
    {
    public:
        GE_MSG_BODY_COMMON_IMPL(WindowFocusMsg)
    };

    class WindowLostFocusMsg : public MsgBodyBase
    {
    public:
        GE_MSG_BODY_COMMON_IMPL(WindowLostFocusMsg)
    };
} // namespace GE