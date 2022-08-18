#include "resource.h"

#include "Runtime/function/Log/LogSystem.h"

namespace GE
{
    void CacheManagerTestCase::run()
    {
        CacheManager::GetInstance().Save({"test1", "test1"}, "test1", 6);
        CacheManager::GetInstance().Save({"test2", "test2"}, "test2", 6);
        CacheManager::GetInstance().Save({"test3", "test3"}, "test3", 6);

        char*    res;
        uint64_t size;
        GE_CORE_ASSERT(CacheManager::GetInstance().Load({"test1", "test1"}, &res, size), "Failed to load test1");
        GE_CORE_ASSERT(size == 6 && strcmp(res, "test1") == 0,
                       "content error, expected test1, got: {}, size: {}",
                       std::string(res),
                       size);
        GE_CORE_ASSERT(!CacheManager::GetInstance().Load({"test?", "test?"}, &res, size), "Wrong content loaded");

        CacheManager::GetInstance().Invalidate([](std::string name, std::string type, std::string specifier) {
            return (name == "test1" && specifier == "test1") || (name == "test2" && specifier == "test2") ||
                   (name == "test3" && specifier == "test3");
        });
        GE_CORE_ASSERT(!CacheManager::GetInstance().Load({"test2", "test2"}, &res, size), "Failed to invalidate");
    }
} // namespace GE