#include "UnitTest.h"

#include "Runtime/core/Log/LogSystem.h"

#include "function/scene.h"
#include "function/shader.h"
#include "resource/resource.h"

#define GE_CHECK_TESTCASE(Testcase) \
    do \
    { \
        GE_CORE_TRACE("Testing: " #Testcase); \
        Testcase testcase; \
        testcase.run(); \
    } while (0)

namespace GE
{
    void TestCase::RunAll()
    {
        GE_CORE_INFO("Running all test cases:");

        GE_CHECK_TESTCASE(CacheManagerTestCase);
        GE_CHECK_TESTCASE(SceneECSTestCase);
        GE_CHECK_TESTCASE(ShaderTestCase);
    }
} // namespace GE