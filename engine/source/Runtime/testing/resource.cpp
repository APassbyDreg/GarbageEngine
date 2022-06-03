#include "resource.h"

#include "function/Log/LogSystem.h"

namespace GE
{
    void CacheManagerTestCase::run()
    {
        CacheManager::GetInstance().Save({"test1", "test1"}, "test1", 5);
        CacheManager::GetInstance().Save({"test2", "test2"}, "test2", 5);
        CacheManager::GetInstance().Save({"test3", "test3"}, "test3", 5);

        char*    res;
        uint64_t size;
        GE_CORE_ASSERT(CacheManager::GetInstance().Load({"test1", "test1"}, &res, size), "Failed to load test1");
        GE_CORE_ASSERT(size == 5 && strcmp(res, "test1") == 0, "content error");
        GE_CORE_ASSERT(!CacheManager::GetInstance().Load({"test?", "test?"}, &res, size), "Wrong content loaded");

        CacheManager::GetInstance().Invalidate([](std::string name, std::string type, std::string specifier) {
            return (name == "test1" && specifier == "test1") || (name == "test2" && specifier == "test2") ||
                   (name == "test3" && specifier == "test3");
        });
        GE_CORE_ASSERT(!CacheManager::GetInstance().Load({"test2", "test2"}, &res, size), "Failed to invalidate");
    }

    void ShaderManagerTestCase::run()
    {
        fs::path fpath = fs::path(Config::shader_dir) / "passes/__test01_simple_triangle/test.frag";
        fs::path vpath = fs::path(Config::shader_dir) / "passes/__test01_simple_triangle/test.vert";

        bool                    use_cache;
        shaderc::CompileOptions options;
        use_cache = true;
        ShaderManager::GetInstance().GetCompiledSpv(fpath.string(), options, use_cache);
        use_cache = true;
        ShaderManager::GetInstance().GetCompiledSpv(vpath.string(), options, use_cache);

        // load cache
        use_cache = true;
        ShaderManager::GetInstance().GetCompiledSpv(vpath.string(), options, use_cache);
        GE_CORE_ASSERT(use_cache, "Failed to load cached spv");

        // clear cache
        ShaderManager::GetInstance().ClearShaderCache();
        use_cache = true;
        ShaderManager::GetInstance().GetCompiledSpv(vpath.string(), options, use_cache);
        GE_CORE_ASSERT(!use_cache, "Failed to clear shader cache");
    }
} // namespace GE