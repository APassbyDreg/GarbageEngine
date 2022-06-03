#include "UnitTest.h"

#include "function/Log/LogSystem.h"
#include "resource.h"

#define GE_CHECK_TESTCASE(Testcase) \
    do \
    { \
        Testcase testcase; \
        testcase.run(); \
    } while (0)

namespace GE
{
    void TestCase::RunAll()
    {
        GE_CHECK_TESTCASE(CacheManagerTestCase);
        GE_CHECK_TESTCASE(ShaderManagerTestCase);
    }
} // namespace GE