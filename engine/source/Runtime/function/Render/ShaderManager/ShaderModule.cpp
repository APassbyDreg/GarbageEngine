#include "ShaderModule.h"

namespace GE
{
    ShaderModule::ShaderModule(std::vector<uint32_t> spv, ShaderType type, const std::string& entry) : m_entry(entry)
    {
        VkShaderModuleCreateInfo create_info = {};
        create_info.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize                 = spv.size() * sizeof(uint32_t);
        create_info.pCode                    = spv.data();

        GE_VK_ASSERT(vkCreateShaderModule(VulkanCore::GetVkDevice(), &create_info, nullptr, &m_module));

        m_stage = VkInit::GetPipelineShaderStageCreateInfo(
            static_cast<VkShaderStageFlagBits>(type), m_module, m_entry.c_str());

        m_ready = true;
    }

    ShaderModule::~ShaderModule()
    {
        if (m_ready)
        {
            vkDestroyShaderModule(VulkanCore::GetVkDevice(), m_module, nullptr);
        }
    }
} // namespace GE