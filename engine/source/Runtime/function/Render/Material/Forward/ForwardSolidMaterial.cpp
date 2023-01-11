#pragma once

#include "ForwardSolidMaterial.h"

#include "Runtime/function/Render/ShaderManager/GLSLCompiler.h"
#include <vcruntime_string.h>

namespace GE
{
    bool ForwardSolidMaterial::IsOpaque() const { return m_color.a == 1.0; }

    void ForwardSolidMaterial::Deserialize(const json& data)
    {
        GE_CORE_CHECK(data["type"].get<std::string>() == GetName(),
                      "[ForwardSolidMaterial::Deserialize] Error input type '{}'",
                      data["type"].get<std::string>());
        if (data["type"].get<std::string>() == GetName())
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
        data["type"]  = GetName();
        data["alias"] = m_alias;
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

    void ForwardSolidMaterial::SetupShadingPass(MaterialRenderPassData data)
    {
        auto&& [cmd, layout] = data;
        vkCmdPushConstants(cmd, layout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float4), &m_color);
    }

    void ForwardSolidMaterial::SetupShadingPipeline(GraphicsRenderPipeline& pipeline)
    {
        auto pc_range = VkInit::GetPushConstantRange(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(float4));
        pipeline.m_pushConstantRanges.push_back(pc_range);

        fs::path     fspath     = fs::path(Config::shader_dir) / "Passes/Forward/Solid.frag";
        GLSLCompiler fscompiler = {ShaderType::FRAGMENT};
        pipeline.m_shaders.push_back(fscompiler.Compile(fspath.string()));
    }
} // namespace GE