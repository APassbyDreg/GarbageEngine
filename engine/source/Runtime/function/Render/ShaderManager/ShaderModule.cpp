#include "ShaderModule.h"

namespace GE
{
    ShaderModule::ShaderModule(std::vector<uint32_t>&& spv, ShaderType type, const std::string& entry) : m_entry(entry)
    {
        /* ------------------------ create VK module ------------------------ */
        VkShaderModuleCreateInfo create_info = {};
        create_info.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize                 = spv.size() * sizeof(uint32_t);
        create_info.pCode                    = spv.data();

        GE_VK_ASSERT(vkCreateShaderModule(VulkanCore::GetDevice(), &create_info, nullptr, &m_module));

        m_stage = VkInit::GetPipelineShaderStageCreateInfo(
            static_cast<VkShaderStageFlagBits>(type), m_module, m_entry.c_str());

        /* --------------------- create reflection data --------------------- */
        m_spvCompiler  = std::make_unique<spirv_cross::Compiler>(std::move(spv));
        auto active    = m_spvCompiler->get_active_interface_variables();
        auto resources = m_spvCompiler->get_shader_resources(active);
        for (auto&& resource : resources.uniform_buffers)
        {
            m_reflectionData[resource.name] = {ShaderResourceType::UNIFORM_BUFFER, resource.id};
        }
        for (auto&& resource : resources.storage_buffers)
        {
            m_reflectionData[resource.name] = {ShaderResourceType::STORAGE_BUFFER, resource.id};
        }
        for (auto&& resource : resources.stage_inputs)
        {
            m_reflectionData[resource.name] = {ShaderResourceType::STAGE_INPUT, resource.id};
        }
        for (auto&& resource : resources.stage_outputs)
        {
            m_reflectionData[resource.name] = {ShaderResourceType::STAGE_OUTPUT, resource.id};
        }
        for (auto&& resource : resources.subpass_inputs)
        {
            m_reflectionData[resource.name] = {ShaderResourceType::SUBPASS_INPUT, resource.id};
        }
        for (auto&& resource : resources.storage_images)
        {
            m_reflectionData[resource.name] = {ShaderResourceType::STORAGE_IMAGE, resource.id};
        }
        for (auto&& resource : resources.sampled_images)
        {
            m_reflectionData[resource.name] = {ShaderResourceType::SAMPLED_IMAGE, resource.id};
        }
        for (auto&& resource : resources.atomic_counters)
        {
            m_reflectionData[resource.name] = {ShaderResourceType::ATOMIC_COUNTER, resource.id};
        }
        for (auto&& resource : resources.acceleration_structures)
        {
            m_reflectionData[resource.name] = {ShaderResourceType::ACCLERAION_STRUCTURE, resource.id};
        }
        for (auto&& resource : resources.push_constant_buffers)
        {
            m_reflectionData[resource.name] = {ShaderResourceType::PUSH_CONSTANT_BUFFER, resource.id};
        }
        for (auto&& resource : resources.separate_images)
        {
            m_reflectionData[resource.name] = {ShaderResourceType::SEPERATE_IMAGE, resource.id};
        }
        for (auto&& resource : resources.separate_samplers)
        {
            m_reflectionData[resource.name] = {ShaderResourceType::SEPERATE_SAMPLER, resource.id};
        }

        /* -------------------------- mark as ready ------------------------- */
        m_ready = true;
    }

    ShaderModule::~ShaderModule()
    {
        if (m_ready)
        {
            vkDestroyShaderModule(VulkanCore::GetDevice(), m_module, nullptr);
        }
    }
} // namespace GE