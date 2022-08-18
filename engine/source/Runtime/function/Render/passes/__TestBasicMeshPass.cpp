#include "__TestBasicMeshPass.h"

#include "Runtime/function/Render/ShaderManager/GLSLCompiler.h"

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
            fs::path     fullpath = fs::path(Config::shader_dir) / "passes/__test02_simple_mesh/test.frag";
            GLSLCompiler compiler = {ShaderType::FRAGMENT};
            m_pipeline.m_shaders.push_back(compiler.Compile(fullpath.string()));
        }
        {
            fs::path     fullpath = fs::path(Config::shader_dir) / "passes/__test02_simple_mesh/test.vert";
            GLSLCompiler compiler = {ShaderType::VERTEX};
            m_pipeline.m_shaders.push_back(compiler.Compile(fullpath.string()));
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
} // namespace GE