#include "PerSceneDataManager.h"

#include "Runtime/Application.h"

#include "../RenderPipeline.h"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <vector>

namespace GE
{
    void PerSceneDataManager::SetupPipeline(GraphicsRenderPipeline& pipeline)
    {
        pipeline.SetDescriptorSetLayout(GE_PerSceneDescriptorSetID, GetPerSceneDataLayout());
    }

    VkDescriptorSetLayout PerSceneDataManager::GetPerSceneDataLayout()
    {
        static DescriptorSetLayout layout;
        if (!layout.IsValid())
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings = {};
            // Light Uniform
            bindings.push_back(
                VkInit::GetDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL, 0));
            // Light Data
            bindings.push_back(
                VkInit::GetDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, 1));
            bindings.push_back(
                VkInit::GetDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, 2));
            bindings.push_back(
                VkInit::GetDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, 3));
            layout.Create(VkInit::GetDescriptorSetLayoutCreateInfo(bindings));
        }
        return layout;
    }

    void PerSceneDataManager::Init(uint n_frames)
    {
        /* --------------------------- light data --------------------------- */
        m_renderResourceManager.ReservePerFramePersistantDescriptorSet("PerSceneData", GetPerSceneDataLayout());
        {
            auto alloc_info  = VkInit::GetAllocationCreateInfo(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                                                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
            auto buffer_info = VkInit::GetBufferCreateInfo(
                sizeof(uint) * 4, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            m_renderResourceManager.ReservePerFrameStaticBuffer("LightUniform", buffer_info, alloc_info);
        }
        {
            auto alloc_info = VkInit::GetAllocationCreateInfo(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                                                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
            auto buffer_info =
                VkInit::GetBufferCreateInfo(0,
                                            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                                                VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            m_renderResourceManager.ReservePerFrameDynamicBuffer(
                "PointLightData", buffer_info, alloc_info, sizeof(PointLightData));
            m_renderResourceManager.ReservePerFrameDynamicBuffer(
                "SpotLightData", buffer_info, alloc_info, sizeof(SpotLightData));
            m_renderResourceManager.ReservePerFrameDynamicBuffer(
                "DirectionLightData", buffer_info, alloc_info, sizeof(DirectionLightData));
        }
        {
            // write to set
            auto&& buffers     = m_renderResourceManager.GetFramewiseStaticBuffer("LightUniform");
            auto&& descriptors = m_renderResourceManager.GetFramewisePersistantDescriptorSet("PerSceneData");
            std::vector<VkWriteDescriptorSet>   writes       = {};
            std::vector<VkDescriptorBufferInfo> buffer_infos = {};
            for (int frame = 0; frame < n_frames; frame++)
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

    void PerSceneDataManager::UpdateData(uint frame_id)
    {
        auto&& sc            = Application::GetInstance().GetActiveScene();
        auto&& light_manager = sc->GetLightManager();

        auto&& point_light_data_buffer = m_renderResourceManager.GetPerFrameDynamicBuffer(frame_id, "PointLightData");
        m_pointLightData               = light_manager.GetPointLightData();
        point_light_data_buffer->UploadAs(m_pointLightData);

        m_spotLightData               = light_manager.GetSpotLightData();
        auto&& spot_light_data_buffer = m_renderResourceManager.GetPerFrameDynamicBuffer(frame_id, "SpotLightData");
        spot_light_data_buffer->UploadAs(m_spotLightData);

        m_directionLightData = light_manager.GetDirectionLightData();
        auto&& direction_light_data_buffer =
            m_renderResourceManager.GetPerFrameDynamicBuffer(frame_id, "DirectionLightData");
        direction_light_data_buffer->UploadAs(m_directionLightData);

        auto&&       light_uniform_buffer = m_renderResourceManager.GetPerFrameStaticBuffer(frame_id, "LightUniform");
        LightUniform light_uniform        = light_manager.GetLightUniformData();
        light_uniform_buffer->Upload((byte*)&light_uniform, sizeof(LightUniform));
        auto&& descriptor_set = m_renderResourceManager.GetPerFramePersistantDescriptorSet(frame_id, "PerSceneData");
        std::vector<VkWriteDescriptorSet>           writes;
        std::vector<VkDescriptorBufferInfo>         buffer_infos;
        std::vector<std::shared_ptr<AutoGpuBuffer>> light_data_buffers = {
            point_light_data_buffer, spot_light_data_buffer, direction_light_data_buffer};
        std::vector<size_t> light_data_sizes = {sizeof(PointLightData) * Max(1u, light_uniform.num_point_lights),
                                                sizeof(SpotLightData) * Max(1u, light_uniform.num_spot_lights),
                                                sizeof(DirectionLightData) *
                                                    Max(1u, light_uniform.num_direction_lights)};
        for (int i = 0; i < 3; i++)
        {
            VkDescriptorBufferInfo buffer_info = {};
            buffer_info.buffer                 = light_data_buffers[i]->GetBuffer();
            buffer_info.offset                 = 0;
            buffer_info.range                  = light_data_sizes[i];
            buffer_infos.push_back(buffer_info);
            VkWriteDescriptorSet write = {};
            write.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet               = descriptor_set;
            write.descriptorType       = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            write.descriptorCount      = 1;
            write.dstBinding           = i + 1;
            write.dstArrayElement      = 0;
            write.pBufferInfo          = &buffer_infos[i];
            writes.push_back(write);
        }
        VulkanCore::WriteDescriptors(writes);
        light_manager.ResetChangedState();
    }
} // namespace GE