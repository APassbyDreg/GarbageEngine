#include "__TestBasicTrianglePass.h"

#include "../VulkanManager/VulkanCreateInfoBuilder.h"

#include "Runtime/function/Render/ShaderManager/GLSLCompiler.h"

namespace GE
{
    void TestBasicTrianglePass::Init()
    {
        m_name = "TestBasicTrianglePass";

        /* ------------------------- setup resources ------------------------ */
        RenderPassResource output = {};
        output.desc.finalLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        m_output.push_back(output);
        __update_resource();

        /* ------------------------ setup render pass ----------------------- */
        Build();

        /* ------------------------- setup pipeline ------------------------- */
        {
            fs::path     fullpath = fs::path(Config::shader_dir) / "Passes/__test01_simple_triangle/test.frag";
            GLSLCompiler compiler = {ShaderType::FRAGMENT};
            m_pipeline.m_shaders.push_back(compiler.Compile(fullpath.string()));
        }
        {
            fs::path     fullpath = fs::path(Config::shader_dir) / "Passes/__test01_simple_triangle/test.vert";
            GLSLCompiler compiler = {ShaderType::VERTEX};
            m_pipeline.m_shaders.push_back(compiler.Compile(fullpath.string()));
        }
        m_pipeline.m_vertexInputState = VkInit::GetPipelineVertexInputStateCreateInfo({}, {});

        m_pipeline.m_inputAssemblyState = VkInit::GetPipelineInputAssemblyStateCreateInfo();

        m_pipeline.m_rasterizationState = VkInit::GetPipelineRasterizationStateCreateInfo();

        m_pipeline.m_multisampleState = VkInit::GetPipelineMultisampleStateCreateInfo();

        VkViewport viewport        = VkInit::GetViewport(m_extent);
        VkRect2D   scissor         = {{0, 0}, m_extent};
        m_pipeline.m_viewportState = VkInit::GetPipelineViewportStateCreateInfo(viewport, scissor);

        m_pipeline.m_colorBlendState = VkInit::GetPipelineColorBlendStateCreateInfo(m_flattenAttachmentBlendStates);

        std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        m_pipeline.m_dynamicState                  = VkInit::GetPipelineDynamicStateCreateInfo(dynamic_states);

        m_pipeline.Build(m_renderPass, 0);
    }

    void TestBasicTrianglePass::Run(VkRenderPassBeginInfo& rp_info, VkCommandBuffer& cmd)
    {
        {
            VkCommandBufferBeginInfo info = {};
            info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            GE_VK_ASSERT(vkBeginCommandBuffer(cmd, &info));
        }

        vkCmdBeginRenderPass(cmd, &rp_info, VK_SUBPASS_CONTENTS_INLINE);

        {
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline());
            vkCmdDraw(cmd, 3, 1, 0, 0);
        }

        vkCmdEndRenderPass(cmd);

        {
            GE_VK_ASSERT(vkEndCommandBuffer(cmd));
        }
    }
} // namespace GE
