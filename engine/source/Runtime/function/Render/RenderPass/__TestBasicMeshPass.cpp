#include "__TestBasicMeshPass.h"

#include "Runtime/function/Render/ShaderManager/GLSLCompiler.h"

namespace GE
{
    void TestBasicMeshPass::InitInternal(uint frame_cnt)
    {
        m_name = "TestBasicMeshPass";

        /* ------------------------- setup resources ------------------------ */
        GraphicsPassResource output = {};
        output.desc.finalLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        m_output.push_back(output);
        __update_resource();

        /* ------------------------- setup pipeline ------------------------- */
        {
            fs::path     fspath     = fs::path(Config::shader_dir) / "Passes/__test02_simple_mesh/test.frag";
            fs::path     vspath     = fs::path(Config::shader_dir) / "Passes/__test02_simple_mesh/test.vert";
            GLSLCompiler fscompiler = {ShaderType::FRAGMENT};
            GLSLCompiler vscompiler = {ShaderType::VERTEX};
            m_pipeline.m_shaders.push_back(vscompiler.Compile(vspath.string()));
            m_pipeline.m_shaders.push_back(fscompiler.Compile(fspath.string()));
        }

        static VertexInputDescription input_desc = Vertex::GetVertexInputDesc();
        m_pipeline.m_vertexInputState =
            VkInit::GetPipelineVertexInputStateCreateInfo(input_desc.bindings, input_desc.attributes, input_desc.flags);

        m_pipeline.m_inputAssemblyState = VkInit::GetPipelineInputAssemblyStateCreateInfo();

        m_pipeline.m_rasterizationState = VkInit::GetPipelineRasterizationStateCreateInfo();

        m_pipeline.m_multisampleState = VkInit::GetPipelineMultisampleStateCreateInfo();

        VkViewport viewport        = VkInit::GetViewport(m_extent);
        VkRect2D   scissor         = {{0, 0}, m_extent};
        m_pipeline.m_viewportState = VkInit::GetPipelineViewportStateCreateInfo(viewport, scissor);

        m_pipeline.m_colorBlendState = VkInit::GetPipelineColorBlendStateCreateInfo(m_flattenAttachmentBlendStates);

        m_pipeline.m_dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        auto push_constant = VkInit::GetPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                                                          sizeof(TestBasicMeshPushConstants));
        m_pipeline.m_pushConstantRanges.push_back(push_constant);
    }

    void TestBasicMeshPass::Run(RenderPassRunData run_data, TestBasicMeshPassData pass_data)
    {
        // unpack data
        auto&& [frame_idx, cmd, rp_info, wait_semaphores, signal_semaphores, fence] = run_data;
        auto&& [vertex_buffer, index_buffer, vertex_cnt, viewport_size]             = pass_data;

        // begin cmd buffer and render pass
        {
            VkCommandBufferBeginInfo info = {};
            info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            GE_VK_ASSERT(vkBeginCommandBuffer(cmd, &info));
            vkCmdBeginRenderPass(cmd, &rp_info, VK_SUBPASS_CONTENTS_INLINE);
        }

        // bind data and draw
        {
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline());

            // set dynamic viewport
            VkViewport viewport = VkInit::GetViewport(viewport_size);
            VkRect2D   scissor  = {{0, 0}, viewport_size};
            vkCmdSetViewport(cmd, 0, 1, &viewport);
            vkCmdSetScissor(cmd, 0, 1, &scissor);

            // set vertex buffer
            VkDeviceSize offset    = 0;
            VkBuffer     buffers[] = {vertex_buffer->GetBuffer()};
            vkCmdBindVertexBuffers(cmd, 0, 1, buffers, &offset);
            vkCmdBindIndexBuffer(cmd, index_buffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

            // set push constants
            TestBasicMeshPushConstants push_constants = {};
            push_constants.mvp                        = float4x4(1.0f);
            push_constants.cameraPosWS                = {0, 0, 0, 0};
            push_constants.debugColor                 = {0.8, 0.5, 0.5, 1.0};
            vkCmdPushConstants(cmd,
                               GetPipelineLayout(),
                               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                               0,
                               sizeof(push_constants),
                               &push_constants);

            // draw
            vkCmdDrawIndexed(cmd, vertex_cnt, 1, 0, 0, 0);
        }

        // end render pass and cmd buffer
        {
            vkCmdEndRenderPass(cmd);
            GE_VK_ASSERT(vkEndCommandBuffer(cmd));
        }

        // submit
        {
            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo         info       = {};
            info.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount         = wait_semaphores.size();
            info.pWaitSemaphores            = wait_semaphores.data();
            info.pWaitDstStageMask          = &wait_stage;
            info.commandBufferCount         = 1;
            info.pCommandBuffers            = &cmd;
            info.signalSemaphoreCount       = signal_semaphores.size();
            info.pSignalSemaphores          = signal_semaphores.data();
            VulkanCore::SubmitToGraphicsQueue(info, fence);
        }
    }
} // namespace GE