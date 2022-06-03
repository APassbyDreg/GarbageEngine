#pragma once

#include "GE_pch.h"

#include "core/Json.h"
#include "core/math/math.h"

#include "CacheManager.h"
#include "core/base/Singleton.h"
#include "shaderc/shaderc.hpp"

namespace GE
{
    class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
    {
    public:
        ShaderIncluder(std::string _shader_path);

        void AddIncludePath(std::string path) { m_includePaths.push_back(path); };

        virtual shaderc_include_result* GetInclude(const char*          requested_source,
                                                   shaderc_include_type type,
                                                   const char*          requesting_source,
                                                   size_t               include_depth) override;

        virtual void ReleaseInclude(shaderc_include_result* data) override;

    private:
        std::string __resolve_path(std::string path, bool relative);

        fs::path              m_shaderPath;
        std::vector<fs::path> m_includePaths;
    };

    class ShaderManager : public Singleton<ShaderManager>
    {
    public:
        std::vector<uint32_t> GetCompiledSpv(std::string path, shaderc::CompileOptions opt, bool& use_cache);

        void ClearShaderCache();

    private:
        shaderc::Compiler m_compiler;
    };
} // namespace GE
