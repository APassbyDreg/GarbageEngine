#include "ColorMappingPass.h"

#include "../ShaderManager/HLSLCompiler.h"

namespace GE
{
    void ColorMappingPass::Init(uint frame_cnt)
    {
        m_frameCnt = frame_cnt;

        // build render pass
        {
            // color
            GraphicsColorResource color_output = {};
            color_output.desc                  = VkInit::GetAttachmentDescription(VK_FORMAT_R8G8B8A8_UNORM,
                                                                 VK_IMAGE_LAYOUT_UNDEFINED,
                                                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                                 VK_ATTACHMENT_LOAD_OP_DONT_CARE);
            color_output.blend                 = VkInit::GetPipelineColorBlendAttachmentState();
            m_renderPass.output.push_back(color_output);
            // build
            m_renderPass.Build();
        }

        // build render pipeline
        if (RenderPipelineManager::HasGraphicsPipeline("ColorMapping"))
        {
            m_pipeline = RenderPipelineManager::GetGraphicsPipeline("ColorMapping");
        }
        else
        {
            m_pipeline = std::make_shared<GraphicsRenderPipeline>();

            // shaders
            {
                fs::path     path     = fs::path(Config::shader_dir) / "Passes/QuadVS.gsf";
                HLSLCompiler compiler = {ShaderType::VERTEX};
                m_pipeline->AddShaderModule(compiler.Compile(path.string(), "VSMain"));
            }
            {
                fs::path     path     = fs::path(Config::shader_dir) / "Passes/ColorMapping/ColorMapping.gsf";
                HLSLCompiler compiler = {ShaderType::FRAGMENT};
                m_pipeline->AddShaderModule(compiler.Compile(path.string(), "FSMain"));
            }

            // descriptor
            VkDescriptorSetLayoutBinding binding = {};
            binding.binding                      = 0;
            binding.descriptorCount              = 1;
            binding.descriptorType               = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            binding.stageFlags                   = VK_SHADER_STAGE_FRAGMENT_BIT;
            m_pipeline->AddDescriptorSetLayoutBinding(0, binding);

            // push constants
            m_pipeline->AddPushConstant("Gamma", VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(float));

            // states
            VertexInputDescription desc = {};
            m_pipeline->m_vertexInputState =
                VkInit::GetPipelineVertexInputStateCreateInfo(desc.bindings, desc.attributes);
            m_pipeline->m_inputAssemblyState =
                VkInit::GetPipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
            m_pipeline->m_multisampleState = VkInit::GetPipelineMultisampleStateCreateInfo();
            VkViewport viewport            = VkInit::GetViewport(GetExtent());
            VkRect2D   scissor             = {{0, 0}, GetExtent()};
            m_pipeline->m_viewportState    = VkInit::GetPipelineViewportStateCreateInfo(viewport, scissor);
            m_pipeline->m_colorBlendState =
                VkInit::GetPipelineColorBlendStateCreateInfo(m_renderPass.GetFlattenColorBlendStates());
            m_pipeline->m_dynamicStates      = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
            m_pipeline->m_depthStencilState  = VkInit::GetPipelineDepthStencilStateCreateInfo(false, false);
            m_pipeline->m_rasterizationState = VkInit::GetPipelineRasterizationStateCreateInfo();

            // build
            m_pipeline->Build(GetRenderPass());
            RenderPipelineManager::RegisterGraphicsPipeline("ColorMapping", m_pipeline);
        }

        // resources
        m_resourceManager.ReservePerFramePersistantDescriptorSet(FullIdentifier("ColorInput"),
                                                                 m_pipeline->GetDescriptorSetLayout(0));
        m_sampler = VkInit::GetSamplerCreateInfo();
    }

    void ColorMappingPass::Resize(uint width, uint height)
    {
        m_extent = {width, height};
        m_frameBuffers.clear();

        auto&& color_imgs = m_resourceManager.GetFramewiseImage("OutputRT");
        for (uint i = 0; i < m_frameCnt; ++i)
        {
            auto&& color_view_info =
                VkInit::GetVkImageViewCreateInfo(color_imgs[i]->GetImageInfo(), VK_IMAGE_ASPECT_COLOR_BIT);
            std::vector<VkImageView> views            = {color_imgs[i]->GetImageView(color_view_info)};
            VkFramebufferCreateInfo  framebuffer_info = {};
            framebuffer_info.sType                    = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass               = GetRenderPass();
            framebuffer_info.attachmentCount          = views.size();
            framebuffer_info.pAttachments             = views.data();
            framebuffer_info.width                    = width;
            framebuffer_info.height                   = height;
            framebuffer_info.layers                   = 1;
            m_frameBuffers.push_back(std::make_shared<FrameBuffer>(framebuffer_info));
        }

        auto&& color_inputs = m_resourceManager.GetFramewiseImage("ColorRT");
        auto&& descriptors  = m_resourceManager.GetFramewisePersistantDescriptorSet(FullIdentifier("ColorInput"));
        for (uint i = 0; i < m_frameCnt; ++i)
        {
            auto view_info =
                VkInit::GetVkImageViewCreateInfo(color_inputs[i]->GetImageInfo(), VK_IMAGE_ASPECT_COLOR_BIT);

            VkDescriptorImageInfo image_info = {};
            image_info.imageLayout           = VK_IMAGE_LAYOUT_GENERAL;
            image_info.imageView             = color_inputs[i]->GetImageView(view_info, true);
            image_info.sampler               = m_sampler;

            VkWriteDescriptorSet write = {};
            write.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet               = descriptors[i];
            write.dstBinding           = 0;
            write.descriptorCount      = 1;
            write.descriptorType       = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.pImageInfo           = &image_info;

            VulkanCore::WriteDescriptors({write});
        }
    }

    void ColorMappingPass::Run(RenderPassRunData run_data, ColorMappingPassData pass_data)
    {
        auto&& [frame_idx, cmd] = run_data;
        auto&& [inv_gamma]      = pass_data;

        // begin render pass
        {
            VkRenderPassBeginInfo rp_info = {};
            rp_info.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rp_info.renderPass            = GetRenderPass();
            rp_info.framebuffer           = m_frameBuffers[frame_idx]->Get();
            rp_info.renderArea.offset     = {0, 0};
            rp_info.renderArea.extent     = m_extent;
            rp_info.clearValueCount       = 0;
            rp_info.pClearValues          = nullptr;
            vkCmdBeginRenderPass(cmd, &rp_info, VK_SUBPASS_CONTENTS_INLINE);
        }

        // setup pipeline
        {
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipeline());

            VkViewport viewport = VkInit::GetViewport(GetExtent());
            VkRect2D   scissor  = {{0, 0}, GetExtent()};
            vkCmdSetViewport(cmd, 0, 1, &viewport);
            vkCmdSetScissor(cmd, 0, 1, &scissor);
        }

        // bind resources
        {
            auto&& descriptor =
                m_resourceManager.GetPerFramePersistantDescriptorSet(frame_idx, FullIdentifier("ColorInput"));
            vkCmdBindDescriptorSets(
                cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipelineLayout(), 0, 1, &descriptor, 0, nullptr);
            m_pipeline->PushConstant("Gamma", cmd, &inv_gamma);
        }

        // draw
        vkCmdDraw(cmd, 3, 1, 0, 0);

        // end render pass
        {
            vkCmdEndRenderPass(cmd);
        }
    }
} // namespace GE