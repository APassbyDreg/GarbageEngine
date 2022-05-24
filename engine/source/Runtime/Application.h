#pragma once

#include "core/LogSystem.h"
#include "macros/macros.h"

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
