#pragma once

#include "GE_pch.h"

#include "core/LogSystem.h"

namespace GE
{
    class GE_API Application
    {
    public:
        void Run();

        Application(/* args */);
        virtual ~Application();
    };

    // Defined in client
    Application* CreateApplication();

} // namespace GE
