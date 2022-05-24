#pragma once

#include "GE_pch.h"

#include "core/LogSystem.h"
#include "core/Window.h"

namespace GE
{
    class GE_API Application
    {
    public:
        void Run();

        Application(/* args */);
        virtual ~Application();

    private:
        std::unique_ptr<Window> m_Window;
    };

    // Defined in client
    Application* CreateApplication();

} // namespace GE
