#include "__TestBasicMeshPass.h"

#include "Runtime/function/Render/ShaderManager/GLSLCompiler.h"
#include "vulkan/vulkan_core.h"
#include <memory>

namespace GE
{
    void TestBasicMeshPass::Resize(uint width, uint height)
    {
        m_extent = {width, height};

        std::vector<std::shared_ptr<GpuImage>> frame_images =
            m_resourceManager.GetFramewiseImage(FullIdentifier("color"));

        uint num_frames = frame_images.size();
        m_frameBuffers.clear();
        for (uint frame_id = 0; frame_id < num_frames; frame_id++)
        {
            // (re)create frame buffer image
            auto&&            img              = frame_images[frame_id];
            VkImageCreateInfo image_info       = {};
            image_info.sType                   = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_info.imageType               = VK_IMAGE_TYPE_2D;
            image_info.extent.width            = width;
            image_info.extent.height           = height;
            image_info.extent.depth            = 1;
            image_info.format                  = VK_FORMAT_R8G8B8A8_UNORM;
            image_info.mipLevels               = 1;
            image_info.arrayLayers             = 1;
            image_info.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
            image_info.usage                   = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            image_info.samples                 = VK_SAMPLE_COUNT_1_BIT;
            image_info.tiling                  = VK_IMAGE_TILING_OPTIMAL;
            image_info.sharingMode             = VK_SHARING_MODE_EXCLUSIVE;
            VmaAllocationCreateInfo alloc_info = {};
            img->Alloc(image_info, alloc_info);

            // (re)create frame buffer
            VkFramebufferCreateInfo framebuffer_info = {};
            VkImageView             img_views[]      = {img->GetImageView()};
            framebuffer_info.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass              = GetRenderPass();
            framebuffer_info.attachmentCount         = 1;
            framebuffer_info.pAttachments            = img_views;
            framebuffer_info.width                   = width;
            framebuffer_info.height                  = height;
            framebuffer_info.layers                  = 1;
            m_frameBuffers.emplace_back(std::make_shared<FrameBuffer>(framebuffer_info));
        }
    }

    void TestBasicMeshPass::InitInternal(uint frame_cnt)
    {
        m_name = "TestBasicMeshPass";

        /* ------------------------ reserve resources ----------------------- */
        m_resourceManager.ReservePerFrameImage(FullIdentifier("color"));

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
        auto&& [frame_idx, cmd, wait_semaphores, signal_semaphores, fence, resource_manager] = run_data;
        auto&& [vertex_buffer, index_buffer, vertex_cnt, viewport_size]             = pass_data;

        // begin cmd buffer and render pass
        VkClearValue clear_value = {};
        clear_value.color        = {0.2f, 0.2f, 0.6f, 1.0f};
        clear_value.depthStencil = {0.0f, 0};
        {
            VkCommandBufferBeginInfo cmd_info = {};
            cmd_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cmd_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            GE_VK_ASSERT(vkBeginCommandBuffer(cmd, &cmd_info));

            VkRenderPassBeginInfo rp_info = {};
            rp_info.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rp_info.renderPass            = GetRenderPass();
            rp_info.framebuffer           = m_frameBuffers[frame_idx]->Get();
            rp_info.renderArea.offset     = {0, 0};
            rp_info.renderArea.extent     = m_extent;
            rp_info.clearValueCount       = 1;
            rp_info.pClearValues          = &clear_value;
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