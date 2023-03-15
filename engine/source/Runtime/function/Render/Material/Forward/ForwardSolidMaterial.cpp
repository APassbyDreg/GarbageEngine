#pragma once

#include "ForwardSolidMaterial.h"

#include "Runtime/function/Render/ShaderManager/HLSLCompiler.h"

#include "Runtime/function/Render/RenderPass/ForwardShadingPass.h"

namespace GE
{
    void ForwardSolidMaterial::SetupRenderPipeline(GraphicsRenderPipeline& pipeline)
    {
        // push constants
        pipeline.AddPushConstant("color", VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(float4));

        // shader
        fs::path     fspath     = fs::path(Config::shader_dir) / "Passes/Forward/Solid.gsf";
        HLSLCompiler fscompiler = {ShaderType::FRAGMENT};
        pipeline.m_shaders.push_back(fscompiler.Compile(fspath.string(), "FSMain"));

        // states
        pipeline.m_rasterizationState = VkInit::GetPipelineRasterizationStateCreateInfo();
    }

    void ForwardSolidMaterial::SetupRenderPass(GraphicsPassBase& pass) {}

    void ForwardSolidMaterial::RunShadingPass(MaterialRenderPassData data)
    {
        auto&& [frame_id, cmd, pass] = data;

        auto& shading_pass = dynamic_cast<ForwardShadingPass&>(pass);
        auto& pipeline     = shading_pass.GetPipelineObject();

        pipeline.PushConstant("color", cmd, &m_color);
    }

    bool ForwardSolidMaterial::IsOpaque() const { return m_color.a == 1.0; }

    void ForwardSolidMaterial::Deserialize(const json& data)
    {
        GE_CORE_CHECK(data["type"].get<std::string>() == GetType(),
                      "[ForwardSolidMaterial::Deserialize] Error input type '{}'",
                      data["type"].get<std::string>());
        if (data["type"].get<std::string>() == GetType())
        {
            m_alias   = data["alias"].get<std::string>();
            m_color.r = data["color"][0].get<float>();
            m_color.g = data["color"][1].get<float>();
            m_color.b = data["color"][2].get<float>();
            m_color.a = data["color"][3].get<float>();
        }
    }

    json ForwardSolidMaterial::Serialize()
    {
        json data;
        data["color"] = {m_color.r, m_color.g, m_color.b, m_color.a};
        return data;
    }

    void ForwardSolidMaterial::Inspect()
    {
        char buf[256];
        strcpy(buf, m_alias.c_str());
        ImGui::InputText("Alias", buf, 256);
        m_alias = buf;

        ImGui::DragFloat4("Color", (float*)(&m_color), 0.005, 0.0, 1.0);
    }
} // namespace GE