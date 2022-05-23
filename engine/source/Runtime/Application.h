#pragma once

#include "macros/macros.h"

namespace GE
{
    class GE_API Application
    {
    private:
        /* data */
    public:
        void Run();

        Application(/* args */);
        virtual ~Application();
    };

    // Defined in client
    Application* CreateApplication();

} // namespace GE
