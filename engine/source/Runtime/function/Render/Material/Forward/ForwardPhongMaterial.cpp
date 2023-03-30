#pragma once

#include "ForwardPhongMaterial.h"

#include "Runtime/function/Render/ShaderManager/HLSLCompiler.h"

#include "Runtime/function/Render/RenderPass/ForwardShadingPass.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    void ForwardPhongMaterial::SetupRenderPipeline(GraphicsRenderPipeline& pipeline)
    {
        // push constants
        pipeline.AddPushConstant("params", VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(PhongParams));

        // shader
        fs::path     fspath     = fs::path(Config::shader_dir) / "Passes/Forward/Phong.gsf";
        HLSLCompiler fscompiler = {ShaderType::FRAGMENT};
        pipeline.AddShaderModule(fscompiler.Compile(fspath.string(), "FSMain"));

        // states
        pipeline.m_rasterizationState = VkInit::GetPipelineRasterizationStateCreateInfo();
    }

    void ForwardPhongMaterial::SetupPassResources(GraphicsPassUnit& pass) {}

    void ForwardPhongMaterial::RunShadingPass(MaterialRenderPassData data)
    {
        auto&& [frame_id, cmd, unit] = data;
        auto&& pipeline              = unit.GetPipeline();
        pipeline.PushConstant("params", cmd, &m_params);
    }

    void ForwardPhongMaterial::Deserialize(const json& data)
    {
        GE_CORE_CHECK(data["type"].get<std::string>() == GetType(),
                      "[ForwardPhongMaterial::Deserialize] Error input type '{}'",
                      data["type"].get<std::string>());
        if (data["type"].get<std::string>() == GetType())
        {
            m_alias     = data["alias"].get<std::string>();
            m_params.kd = JsonUtils::LoadFloat3(data["kd"]);
            m_params.ks = JsonUtils::LoadFloat3(data["ks"]);
            m_params.ka = JsonUtils::LoadFloat3(data["ka"]);
            m_params.ns = data["ns"].get<float>();
        }
    }

    json ForwardPhongMaterial::Serialize() const
    {
        json data;
        data["kd"] = JsonUtils::DumpFloat3(m_params.kd);
        data["ks"] = JsonUtils::DumpFloat3(m_params.ks);
        data["ka"] = JsonUtils::DumpFloat3(m_params.ka);
        data["ns"] = m_params.ns;
        return data;
    }

    void ForwardPhongMaterial::Inspect()
    {
        char buf[256];
        strcpy(buf, m_alias.c_str());
        ImGui::InputText("Alias", buf, 256);
        m_alias = buf;

        ImGui::DragFloat3("Kd", (float*)(&m_params.kd), 0.005, 0.0, 1.0);
        ImGui::DragFloat3("Ks", (float*)(&m_params.ks), 0.005, 0.0, 1.0);
        ImGui::DragFloat3("Ka", (float*)(&m_params.ka), 0.005, 0.0, 1.0);
        ImGui::DragFloat("Ns", &m_params.ns, 0.005, 0.0, 1000.0);
    }
} // namespace GE