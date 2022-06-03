#pragma once

#include "GE_pch.h"

namespace GE
{
    class GE_API TestCase
    {
    public:
        virtual void run() = 0;
        static void  RunAll();
    };

} // namespace GE