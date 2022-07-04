#include "__TestBasicTrianglePass.h"

#include "../VulkanManager/VulkanCreateInfoBuilder.h"

namespace GE
{
    TestBasicTrianglePass::TestBasicTrianglePass() {}

    TestBasicTrianglePass::~TestBasicTrianglePass() {}

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
        bool                     use_cache    = false;
        std::vector<std::string> shader_paths = {"passes/__test01_simple_triangle/test.frag",
                                                 "passes/__test01_simple_triangle/test.vert"};
        for (auto&& path : shader_paths)
        {
            fs::path fullpath = fs::path(Config::shader_dir) / path;
            m_pipeline.m_shaders.push_back(
                ShaderManager::GetInstance().GetCompiledModule(fullpath.string(), {}, use_cache));
        }
        m_pipeline.m_vertexInputState = VkInit::GetPipelineVertexInputStateCreateInfo({}, {});

        m_pipeline.m_inputAssemblyState = VkInit::GetPipelineInputAssemblyStateCreateInfo();

        m_pipeline.m_rasterizationState = VkInit::GetPipelineRasterizationStateCreateInfo();

        m_pipeline.m_multisampleState = VkInit::GetPipelineMultisampleStateCreateInfo();

        VkViewport viewport        = VkInit::GetViewport(m_extent);
        VkRect2D   scissor         = {{0, 0}, m_extent};
        m_pipeline.m_viewportState = VkInit::GetPipelineViewportStateCreateInfo(viewport, scissor);

        m_pipeline.m_colorBlendState = VkInit::GetPipelineColorBlendStateCreateInfo(m_flattenAttachmentBlendStates);

        m_pipeline.Build(m_renderPass, 0);
    }
} // namespace GE
