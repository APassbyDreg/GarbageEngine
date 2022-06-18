#pragma once

#include "GE_pch.h"

#include "core/Json.h"
#include "core/base/Singleton.h"
#include "core/math/math.h"

#include "resource/Managers/CacheManager.h"

#include "VulkanCommon.h"

#include "shaderc/shaderc.hpp"

namespace GE
{
    class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
    {
    public:
        ShaderIncluder(std::string _shader_path);
        ShaderIncluder(std::string _shader_path, std::vector<std::string> additional_include_dirs);
        virtual ~ShaderIncluder() = default;

        void AddIncludePath(std::string path) { m_localIncludePaths.push_back(path); };

        virtual shaderc_include_result* GetInclude(const char*          requested_source,
                                                   shaderc_include_type type,
                                                   const char*          requesting_source,
                                                   size_t               include_depth) override;

        virtual void ReleaseInclude(shaderc_include_result* data) override;

    private:
        std::string __resolve_path(std::string path, bool relative);

        fs::path                     m_shaderPath;
        std::vector<fs::path>        m_localIncludePaths;
        static std::vector<fs::path> s_globalIncludePaths;
    };

    class ShaderModule
    {
    public:
        ShaderModule(std::vector<uint32_t> spv);
        ~ShaderModule();

        void Compile(shaderc::Compiler&& compiler);

        inline VkShaderModule GetShaderModule() { return m_compiled ? m_module : VK_NULL_HANDLE; }

    private:
        bool           m_compiled = false;
        VkShaderModule m_module;
    };

    class ShaderManager : public Singleton<ShaderManager>
    {
    public:
        ShaderModule
        GetCompiledModule(std::string path, std::vector<std::string> additional_include_dirs, bool& use_cache);
        std::vector<uint32_t> GetCompiledSpv(std::string path, shaderc::CompileOptions opt, bool& use_cache);
        void                  ClearShaderCache();

    private:
        shaderc_shader_kind __shader_kind_from_ext(std::string filepath);

    private:
        shaderc::Compiler m_compiler;
    };
} // namespace GE
