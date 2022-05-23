#include "Application.h"
#include <stdio.h>

namespace GE
{
    Application::Application() {}

    Application::~Application() {}

    void Application::Run()
    {
        printf("Application::Run()\n");
        while (true)
        {}
    }
} // namespace GE
