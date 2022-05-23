#pragma once
#include "../Application.h"

#ifdef GE_PLATFORM_WINDOWS
extern GE::Application* GE::CreateApplication();

int main(int argc, char const* argv[])
{
    GE::Application* app = GE::CreateApplication();
    app->Run();
    delete app;
    return 0;
}
#else
#error "Unsupported platform"
#endif