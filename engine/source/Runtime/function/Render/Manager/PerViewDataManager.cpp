#include "PerViewDataManager.h"

#include "Runtime/Application.h"

#include "../RenderPipeline.h"

namespace GE
{
    void PerViewDataManager::SetupPipeline(GraphicsRenderPipeline& pipeline)
    {
        pipeline.SetDescriptorSetLayout(GE_PerViewDescriptorSetID, GetPerViewDataLayout());
    }

    VkDescriptorSetLayout PerViewDataManager::GetPerViewDataLayout()
    {
        static DescriptorSetLayout layout;
        if (!layout.IsValid())
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings = {};
            // View Uniform
            bindings.push_back(
                VkInit::GetDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL, 0));
            layout.Create(VkInit::GetDescriptorSetLayoutCreateInfo(bindings));
        }
        return layout;
    }

    void PerViewDataManager::Init(uint n_frames) { m_frameCnt = n_frames; }

    void PerViewDataManager::RegisterView(std::string name)
    {
        auto buffer_identifier = std::format("ViewUniform/{}", name);
        {
            auto alloc_info  = VkInit::GetAllocationCreateInfo(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                                                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
            auto buffer_info = VkInit::GetBufferCreateInfo(
                sizeof(ViewUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            m_renderResourceManager.ReservePerFrameStaticBuffer(buffer_identifier, buffer_info, alloc_info);
        }

        auto descriptor_identifier = std::format("PerViewData/{}", name);
        {
            m_renderResourceManager.ReservePerFramePersistantDescriptorSet(descriptor_identifier,
                                                                           GetPerViewDataLayout());
            auto&& buffers     = m_renderResourceManager.GetFramewiseStaticBuffer(buffer_identifier);
            auto&& descriptors = m_renderResourceManager.GetFramewisePersistantDescriptorSet(descriptor_identifier);
            std::vector<VkWriteDescriptorSet>   writes       = {};
            std::vector<VkDescriptorBufferInfo> buffer_infos = {};
            for (int frame = 0; frame < m_frameCnt; frame++)
            {
                VkDescriptorBufferInfo buffer_info = {};
                buffer_info.buffer                 = buffers[frame]->GetBuffer();
                buffer_info.offset                 = 0;
                buffer_info.range                  = VK_WHOLE_SIZE;
                buffer_infos.push_back(buffer_info);
                VkWriteDescriptorSet write = {};
                write.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet               = descriptors[frame];
                write.descriptorType       = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.descriptorCount      = 1;
                write.dstBinding           = 0;
                write.dstArrayElement      = 0;
                write.pBufferInfo          = &buffer_infos[frame];
                writes.push_back(write);
            }
            VulkanCore::WriteDescriptors(writes);
        }
    }

    void PerViewDataManager::UpdateView(uint frame, std::string name, ViewUniform& data)
    {
        auto  buffer_identifier = std::format("ViewUniform/{}", name);
        auto& buffer            = m_renderResourceManager.GetPerFrameStaticBuffer(frame, buffer_identifier);
        buffer->Upload((byte*)&data, sizeof(ViewUniform));
    }
} // namespace GE