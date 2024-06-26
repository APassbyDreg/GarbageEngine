#pragma once

#include "GE_pch.h"

#include "Runtime/core/Base/Singleton.h"
#include "Runtime/core/Json.h"
#include "Runtime/core/Math/Math.h"

#include "Runtime/resource/Managers/CacheManager.h"

#include "Runtime/function/Render/VulkanManager/VulkanCore.h"
#include "Runtime/function/Render/VulkanManager/VulkanCreateInfoBuilder.h"

#include "spirv_cross/spirv.hpp"
#include "spirv_cross/spirv_common.hpp"
#include "spirv_cross/spirv_cross.hpp"
#include <stdint.h>

namespace GE
{
    // NOTE: copied from VkShaderStageFlagBits
    namespace __Warpper
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
    typedef __Warpper::ShaderType ShaderType;

    // NOTE: one-to-one mapping with spirv_cross::ShaderResources
    enum class ShaderResourceType
    {
        UNIFORM_BUFFER,
        STORAGE_BUFFER,
        STAGE_INPUT,
        STAGE_OUTPUT,
        SUBPASS_INPUT,
        STORAGE_IMAGE,
        SAMPLED_IMAGE,
        ATOMIC_COUNTER,
        ACCLERAION_STRUCTURE,
        PUSH_CONSTANT_BUFFER,
        SEPERATE_IMAGE,
        SEPERATE_SAMPLER,
        // BUILTIN_INPUT,
        // BUILTIN_OUTPUT,
    };

    class ShaderModule
    {
        struct ShaderResourceInfo
        {
            ShaderResourceType type;
            spirv_cross::ID    id;
        };

    public:
        ShaderModule(std::vector<uint32_t>&& spv, ShaderType type, const std::string& entry = "main");
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
        std::string                     m_entry = "main";
        VkShaderModule                  m_module;
        VkPipelineShaderStageCreateInfo m_stage;

        std::map<std::string, ShaderResourceInfo> m_reflectionData;
        std::unique_ptr<spirv_cross::Compiler>    m_spvCompiler = nullptr;
    };

} // namespace GE
