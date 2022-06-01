#pragma once

#include "GE_pch.h"

#include "../Application.h"
#include "../testing.h"

#ifdef GE_PLATFORM_WINDOWS
extern std::unique_ptr<GE::Application> GE::CreateApplication();

int main(int argc, char const* argv[])
{
#ifdef GE_ENABLE_TESTING
    GE::Testing::test();
#endif

    std::unique_ptr<GE::Application> app = GE::CreateApplication();
    app->Run();
    return 0;
}
#else
#error "Unsupported platform"
#endif