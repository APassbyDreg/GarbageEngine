#include "AABBCullPass.h"

#include "../VulkanManager/VulkanCore.h"
#include "../VulkanManager/VulkanCreateInfoBuilder.h"

#include "../ShaderManager/GLSLCompiler.h"

#include "Runtime/Application.h"

#include "Runtime/function/Scene/Logic/EntityAABB.h"
#include "Runtime/function/Scene/Scene.h"
#include <memory>

namespace GE
{
    void AABBCullPass::InitInternal(uint frame_cnt)
    {
        m_name = "AABBCullPass";

        /* ------------------------- setup pipeline ------------------------- */
        VkDescriptorSetLayout set_layout;
        {
            // shader
            fs::path     fullpath = fs::path(Config::shader_dir) / "Passes/AABBCull/AABBCull.cs.glsl";
            GLSLCompiler compiler = {ShaderType::COMPUTE};
            m_pipeline.m_shader   = compiler.Compile(fullpath.string());
            // layout
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            bindings.push_back(VkInit::GetDescriptorSetLayoutBinding(
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 0));
            bindings.push_back(VkInit::GetDescriptorSetLayoutBinding(
                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, VK_SHADER_STAGE_COMPUTE_BIT, 1));
            auto info  = VkInit::GetDescriptorSetLayoutCreateInfo(bindings);
            m_layout   = std::make_shared<DescriptorSetLayout>(info);
            set_layout = m_layout->Get();
            m_pipeline.m_descriptorSetLayout.push_back(set_layout);
            // push constant
            m_pipeline.m_pushConstantRanges.push_back(
                VkInit::GetPushConstantRange(VK_SHADER_STAGE_COMPUTE_BIT, sizeof(AABBCullPassData), 0));
        }

        /* ------------------------- setup resources ------------------------ */
        const uint default_instance_cnt = 256;
        auto       layouts              = std::vector<VkDescriptorSetLayout>(1, set_layout);
        auto       desc_sets_alloc_info = VkInit::GetDescriptorSetAllocateInfo(layouts);
        {
            VmaAllocationCreateInfo alloc_info = {};
            alloc_info.usage                   = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            VkBufferCreateInfo buffer_info     = {};
            buffer_info.sType                  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_info.size                   = default_instance_cnt * sizeof(Bounds3f);
            buffer_info.usage                  = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            m_resourceManager.ReservePerFrameDynamicBuffer(FullIdentifier("AabbBuffer"), buffer_info, alloc_info);
        }
        {
            VmaAllocationCreateInfo alloc_info = {};
            alloc_info.usage                   = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
            VkBufferCreateInfo buffer_info     = {};
            buffer_info.sType                  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_info.size                   = ceil(default_instance_cnt / 8.0 / sizeof(uint)) * sizeof(uint);
            buffer_info.usage                  = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            m_resourceManager.ReservePerFrameDynamicBuffer(FullIdentifier("ResultBuffer"), buffer_info, alloc_info);
        }
        m_resourceManager.ReservePerFrameDescriptorSet(FullIdentifier("Descriptor"), desc_sets_alloc_info);
    }

    void AABBCullPass::Run(RenderPassRunData& run_data, AABBCullPassData& data)
    {
        // unpack data
        auto&& [frame_idx, cmd, wait_semaphores, signal_semaphores, fence] = run_data;
        auto&& aabb_buffer   = m_resourceManager.GetPerFrameDynamicBuffer(frame_idx, FullIdentifier("AabbBuffer"));
        auto&& result_buffer = m_resourceManager.GetPerFrameDynamicBuffer(frame_idx, FullIdentifier("ResultBuffer"));
        auto&& desc_set      = m_resourceManager.GetPerFrameDescriptorSet(frame_idx, "Descriptor");

        // get AABB of all renderable objects
        auto                  sc          = Application::GetInstance().GetActiveScene();
        auto                  renderables = sc->GetMeshManager().FrustumCull(data.camera_vp);
        uint                  num_aabbs   = renderables.size();
        std::vector<Bounds3f> aabbs {renderables.size()};
        for (auto&& e : renderables)
        {
            aabbs.push_back(EntityAABBLogic::GetInstance().GetAABB(*e));
        }

        // update buffers
        // TODO: update only changed AABBs
        size_t result_size = ceil(num_aabbs / 8.0 / sizeof(uint)) * sizeof(uint);
        aabb_buffer->Upload((byte*)aabbs.data(), aabbs.size() * sizeof(Bounds3f), 0, true);
        result_buffer->Resize(result_size);

        // update descriptor set
        {
            VkDescriptorBufferInfo aabb_buffer_info   = {};
            aabb_buffer_info.buffer                   = aabb_buffer->GetBuffer();
            aabb_buffer_info.offset                   = 0;
            aabb_buffer_info.range                    = VK_WHOLE_SIZE;
            VkDescriptorBufferInfo result_buffer_info = {};
            result_buffer_info.buffer                 = result_buffer->GetBuffer();
            result_buffer_info.offset                 = 0;
            result_buffer_info.range                  = VK_WHOLE_SIZE;
            std::vector<VkWriteDescriptorSet> writes(2);
            writes[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[0].dstSet          = desc_set;
            writes[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writes[0].descriptorCount = 1;
            writes[0].dstBinding      = 0;
            writes[0].dstArrayElement = 0;
            writes[0].pBufferInfo     = &aabb_buffer_info;
            writes[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[1].dstSet          = desc_set;
            writes[1].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            writes[1].descriptorCount = 1;
            writes[1].dstBinding      = 1;
            writes[1].dstArrayElement = 0;
            writes[1].pBufferInfo     = &result_buffer_info;
            VulkanCore::WriteDescriptors(writes);
        }

        // bind, dispatch and end
        {
            VkCommandBufferBeginInfo info = {};
            info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            GE_VK_ASSERT(vkBeginCommandBuffer(cmd, &info));

            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, GetPipeline());

            vkCmdBindDescriptorSets(
                cmd, VK_PIPELINE_BIND_POINT_COMPUTE, GetPipelineLayout(), 0, 1, &desc_set, 0, nullptr);

            AABBCullPassPushConstants pc = {data.camera_vp, num_aabbs};
            vkCmdPushConstants(
                cmd, GetPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(AABBCullPassPushConstants), &pc);

            vkCmdDispatch(cmd, num_aabbs, 1, 1);

            GE_VK_ASSERT(vkEndCommandBuffer(cmd));
        }

        // submit
        {
            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            VkSubmitInfo         info       = {};
            info.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount         = wait_semaphores.size();
            info.pWaitSemaphores            = wait_semaphores.data();
            info.pWaitDstStageMask          = &wait_stage;
            info.commandBufferCount         = 1;
            info.pCommandBuffers            = &cmd;
            info.signalSemaphoreCount       = signal_semaphores.size();
            info.pSignalSemaphores          = signal_semaphores.data();
            VulkanCore::SubmitToComputeQueue(info, fence);
        }
    }
} // namespace GE
