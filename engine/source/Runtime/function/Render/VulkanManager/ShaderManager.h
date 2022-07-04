#pragma once

#include "GE_pch.h"

#include "Runtime/core/Json.h"
#include "Runtime/core/base/Singleton.h"
#include "Runtime/core/math/math.h"

#include "Runtime/resource/Managers/CacheManager.h"

#include "VulkanCommon.h"

#include "shaderc/shaderc.hpp"

namespace GE
{
    // NOTE: copied from VkShaderStageFlagBits
    namespace __Warper
    {
        enum ShaderType
        {
            ALL = 0x7FFFFFFF,

            /* ----------------------------- common ----------------------------- */

            VERTEX                  = 0x00000001,
            TESSELLATION_CONTROL    = 0x00000002,
            TESSELLATION_EVALUATION = 0x00000004,
            GEOMETRY                = 0x00000008,
            FRAGMENT                = 0x00000010,
            ALL_GRAPHICS            = 0x0000001F,
            COMPUTE                 = 0x00000020,

            /* ----------------------------- nvidia ----------------------------- */

            TASK = 0x00000040,
            MESH = 0x00000080,

            /* --------------------------- ray-tracing -------------------------- */

            RAY_GENERATION   = 0x00000100,
            RAY_ANYHIT       = 0x00000200,
            RAY_CLOSESTHIT   = 0x00000400,
            RAY_MISS         = 0x00000800,
            RAY_INTERSECTION = 0x00001000,
            RAY_CALLABLE     = 0x00002000,
        };
    }
    typedef __Warper::ShaderType ShaderType;

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
        ShaderModule(std::vector<uint32_t> spv, ShaderType type);
        ~ShaderModule();

        inline VkShaderModule GetShaderModule()
        {
            GE_CORE_ASSERT(m_ready, "ShaderModule::GetShaderModule: shader is not compiled!");
            return m_module;
        }
        inline VkPipelineShaderStageCreateInfo GetShaderStage()
        {
            GE_CORE_ASSERT(m_ready, "ShaderModule::GetShaderStage: shader is not compiled");
            return m_stage;
        }

    private:
        bool                            m_ready = false;
        VkShaderModule                  m_module;
        VkPipelineShaderStageCreateInfo m_stage;
    };

    class ShaderManager : public Singleton<ShaderManager>
    {
    public:
        std::shared_ptr<ShaderModule>
        GetCompiledModule(std::string path, std::vector<std::string> additional_include_dirs, bool& use_cache);
        std::vector<uint32_t> GetCompiledSpv(std::string path, shaderc::CompileOptions opt, bool& use_cache);
        void                  ClearShaderCache();

    private:
        shaderc_shader_kind __shader_kind_from_ext(std::string filepath);

    private:
        shaderc::Compiler m_compiler;
    };
} // namespace GE
