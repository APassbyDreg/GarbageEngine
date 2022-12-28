#include "__TestBasicMeshPass.h"

#include "Runtime/function/Render/ShaderManager/GLSLCompiler.h"
#include "Runtime/function/Render/ShaderManager/HLSLCompiler.h"

namespace GE
{
    void TestBasicMeshPass::Init()
    {
        m_name = "TestBasicMeshPass";

        /* ------------------------- setup resources ------------------------ */
        RenderPassResource output = {};
        output.desc.finalLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        m_output.push_back(output);
        __update_resource();

        /* ------------------------ setup render pass ----------------------- */
        Build();

        /* ------------------------- setup pipeline ------------------------- */
        {
            fs::path     fullpath   = fs::path(Config::shader_dir) / "Passes/__test02_simple_mesh/test.hlsl";
            HLSLCompiler vscompiler = {ShaderType::VERTEX};
            HLSLCompiler fscompiler = {ShaderType::FRAGMENT};
            m_pipeline.m_shaders.push_back(vscompiler.Compile(fullpath.string(), "vert"));
            m_pipeline.m_shaders.push_back(fscompiler.Compile(fullpath.string(), "frag"));
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

        std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        m_pipeline.m_dynamicState                  = VkInit::GetPipelineDynamicStateCreateInfo(dynamic_states);

        VkPushConstantRange push_constant = {};
        push_constant.offset              = 0;
        push_constant.size                = sizeof(TestBasicMeshPushConstants);
        push_constant.stageFlags          = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        m_pipeline.m_pushConstantRanges.push_back(push_constant);

        m_pipeline.Build(m_renderPass, 0);
    }

    void TestBasicMeshPass::Run(VkExtent2D&                viewport_size,
                                VkRenderPassBeginInfo&     rp_info,
                                VkCommandBuffer&           cmd,
                                std::shared_ptr<GpuBuffer> vertex_buffer,
                                std::shared_ptr<GpuBuffer> index_buffer,
                                uint                       vertex_cnt)
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

        vkCmdEndRenderPass(cmd);

        {
            GE_VK_ASSERT(vkEndCommandBuffer(cmd));
        }
    }
} // namespace GE