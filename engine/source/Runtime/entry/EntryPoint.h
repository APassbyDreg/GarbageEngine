#pragma once

#include "GE_pch.h"

#include "Runtime/Application.h"
#include "Runtime/testing/UnitTest.h"

#ifdef GE_ENTRYPOINT_CPP

extern std::unique_ptr<GE::Application> GE::CreateApplication();

int main(int argc, char const* argv[])
{
    std::unique_ptr<GE::Application> app = GE::CreateApplication();

#ifdef GE_ENABLE_TESTING
    GE::TestCase::RunAll();
#endif

    app->Run();
    return 0;
}

#endif