#include "TriangleMesh.h"

#include "../ShaderManager/HLSLCompiler.h"

#include "Runtime/function/Scene/Components/Transform.h"

#include "Runtime/resource/Managers/ResourceManager.h"
#include "Runtime/resource/ResourceTypes/TriangleMeshResource.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "Runtime/Application.h"
#include "glm/matrix.hpp"
#include "vulkan/vulkan_core.h"
#include <vector>

namespace GE
{
    VkDescriptorSetLayout TriangleMesh::GetInstanceDataDescriptorSetLayout()
    {
        static std::shared_ptr<DescriptorSetLayout> layout = nullptr;
        if (layout == nullptr)
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            bindings.push_back(VkInit::GetDescriptorSetLayoutBinding(
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, 0));
            auto&& info = VkInit::GetDescriptorSetLayoutCreateInfo(bindings);
            layout      = std::make_shared<DescriptorSetLayout>(info);
        }
        return layout->Get();
    }

    void TriangleMesh::Activate()
    {
        GE_CORE_ASSERT(m_meshResource != nullptr, "Error trying to activate an invalid TriangleMesh");

        m_tLastUsed = Time::CurrentTime();

        if (!m_updateJob.IsRunning())
        {
            m_updateJob.Run(std::bind(&TriangleMesh::Update, this), c_updateInterval);
        }

        if (!m_uploaded)
        {
            auto& data = m_meshResource->GetData();

            auto alloc_info = VkInit::GetAllocationCreateInfo(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                                                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
            auto vertex_info =
                VkInit::GetBufferCreateInfo(sizeof(Vertex) * data.GetVertexCount(),
                                            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
            auto index_info =
                VkInit::GetBufferCreateInfo(sizeof(uint32_t) * data.GetIndexCount(),
                                            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

            m_vertexBuffer.Alloc(vertex_info, alloc_info);
            m_indexBuffer.Alloc(index_info, alloc_info);

            m_vertexBuffer.UploadAs(data.m_vertices);
            m_indexBuffer.UploadAs(data.m_indices);
            m_uploaded = true;
        }
    }

    void TriangleMesh::Deactivate()
    {
        m_vertexBuffer.Destroy();
        m_indexBuffer.Destroy();
        m_uploaded = false;
    }

    bool TriangleMesh::Update()
    {
        if (m_uploaded && Time::CurrentTime() - m_tLastUsed > c_deactiveThres)
        {
            // Deactivate();
        }
        return false;
    }

    Bounds3f& TriangleMesh::BBox() { return m_meshResource->BBox(); }

    void TriangleMesh::SetupRenderPass(std::shared_ptr<GraphicsPass> pass)
    {
        // setup pipeline
        auto&& pipeline = pass->GetPipelineObject();
        {
            fs::path     path     = fs::path(Config::shader_dir) / "Passes/Mesh/TriangleMesh.gsf";
            HLSLCompiler compiler = {
                ShaderType::VERTEX, {}, {std::format("MAX_INSTANCE_COUNT={}", c_maxInstancePerDraw)}};
            pipeline.m_shaders.push_back(compiler.Compile(path.string(), "VSMain"));
        }
        {
            VertexInputDescription& desc = Vertex::GetVertexInputDesc();
            pipeline.m_vertexInputState =
                VkInit::GetPipelineVertexInputStateCreateInfo(desc.bindings, desc.attributes, desc.flags);
            pipeline.m_inputAssemblyState = VkInit::GetPipelineInputAssemblyStateCreateInfo();
        }
        pipeline.m_descriptorSetLayout.push_back(GetInstanceDataDescriptorSetLayout());

        // setup resource
        RenderResourceManager& resource_manager = pass->GetResourceManager();
        {
            // buffer
            auto buffer_info =
                VkInit::GetBufferCreateInfo(64,
                                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            auto alloc_info = VkInit::GetAllocationCreateInfo(VMA_MEMORY_USAGE_AUTO,
                                                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
            resource_manager.ReservePerFrameDynamicBuffer(
                pass->FullIdentifier("InstanceData"), buffer_info, alloc_info);
            // descriptor set
            std::vector<VkDescriptorSetLayout> layouts  = {GetInstanceDataDescriptorSetLayout()};
            VkDescriptorSetAllocateInfo        set_info = VkInit::GetDescriptorSetAllocateInfo(layouts);
            resource_manager.ReservePerFramePersistantDescriptorSet(pass->FullIdentifier("InstanceData"), set_info);
        }
        resource_manager.SetState(pass->FullIdentifier("InstanceDataDescriptorID"),
                                  pipeline.m_descriptorSetLayout.size() - 1);
    }

    void TriangleMesh::RunRenderPass(MeshRenderPassData data)
    {
        Activate();

        auto&& [frame_idx, cmd, layout, instances, pass] = data;
        RenderResourceManager& resource_manager          = pass->GetResourceManager();

        // true instance count
        uint num_instance = instances.size();

        // pad instances and upload to buffer
        std::vector<VertexInstanceData> instance_data;
        for (auto&& instance : instances)
        {
            float4x4 model_mat       = instance->GetComponent<TransformComponent>().GetTransformMatrix();
            float4x4 t_inv_model_mat = glm::transpose(glm::inverse(model_mat));
            instance_data.push_back({model_mat, t_inv_model_mat});
        }
        while (instance_data.size() % c_maxInstancePerDraw != 0)
        {
            instance_data.push_back({});
        }
        auto&& instance_buffer =
            resource_manager.GetPerFrameDynamicBuffer(frame_idx, pass->FullIdentifier("InstanceData"));
        instance_buffer->UploadAs(instance_data);

        // bind vertex buffer and index buffer
        {
            VkDeviceSize offsets[] = {0};
            VkBuffer     buffers[] = {m_vertexBuffer.GetBuffer()};
            vkCmdBindVertexBuffers(cmd, c_vertexBindId, 1, buffers, offsets);
            vkCmdBindIndexBuffer(cmd, m_indexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }

        // write instance descriptor and dispatch
        VkDescriptorSet instance_data_descriptor =
            resource_manager.GetPerFramePersistantDescriptorSet(frame_idx, pass->FullIdentifier("InstanceData"));
        uint instance_data_descriptor_id =
            resource_manager.GetState<uint>(pass->FullIdentifier("InstanceDataDescriptorID"));
        for (uint start_id = 0; start_id < num_instance; start_id += c_maxInstancePerDraw)
        {
            {
                VkDescriptorBufferInfo instance_buffer_info = {};
                instance_buffer_info.buffer                 = instance_buffer->GetBuffer();
                instance_buffer_info.offset                 = start_id * sizeof(VertexInstanceData);
                instance_buffer_info.range                  = sizeof(VertexInstanceData);
                VkWriteDescriptorSet write                  = {};
                write.sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet                                = instance_data_descriptor;
                write.descriptorType                        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.descriptorCount                       = 1;
                write.dstBinding                            = 0;
                write.dstArrayElement                       = 0;
                write.pBufferInfo                           = &instance_buffer_info;
                VulkanCore::WriteDescriptors({write});

                vkCmdBindDescriptorSets(cmd,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        layout,
                                        instance_data_descriptor_id,
                                        1,
                                        &instance_data_descriptor,
                                        0,
                                        nullptr);
            }

            uint num_dispatch = Min(num_instance - start_id, c_maxInstancePerDraw);
            vkCmdDrawIndexed(cmd, m_meshResource->GetIndexCount(), num_dispatch, 0, 0, 0);
        }
    }

    void TriangleMesh::Inspect()
    {
        // alias
        char buf[256];
        strcpy(buf, m_alias.c_str());
        ImGui::InputText("Alias", buf, 256);
        m_alias = buf;

        // mesh file selection
        if (ImGui::Button("Select Triangle Mesh"))
        {
            Application& app     = Application::GetInstance();
            fs::path     workdir = app.GetWorkDirectory();
            ImGuiFileDialog::Instance()->OpenDialog(
                "ChooseFileDlgKey", "Choose File", ".ge.trianglemesh,.*", workdir.string().c_str());
        }
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filepath = ImGuiFileDialog::Instance()->GetFilePathName();
                if (filepath.ends_with(".ge.trianglemesh"))
                {
                    m_meshResource = ResourceManager::GetResource<TriangleMeshResource>(filepath);
                }
                else if (filepath.ends_with(".obj"))
                {
                    std::string target_filepath = filepath.substr(0, filepath.size() - 4) + ".ge.trianglemesh";
                    m_meshResource              = ResourceManager::GetResource<TriangleMeshResource>(target_filepath);
                    m_meshResource->FromObj(filepath);
                }
            }
            ImGuiFileDialog::Instance()->Close();
        }

        // mesh file name
        std::string mesh_path = "Unset";
        if (m_meshResource != nullptr)
        {
            mesh_path = m_meshResource->GetFilePath().string();
        }
        ImGui::SameLine();
        ImGui::Text("Source File: %s", mesh_path.c_str());

        // mesh info
        if (m_meshResource != nullptr)
        {
            auto&& data = m_meshResource->GetData();
            ImGui::Text("%d vertices, %d indices", data.GetVertexCount(), data.GetIndexCount());
        }
    }

    json TriangleMesh::Serialize()
    {
        json data;
        data["alias"] = m_alias;
        data["mesh"]  = m_meshResource == nullptr ? "" : m_meshResource->GetFilePath().string();
        return data;
    }

    void TriangleMesh::Deserialize(const json& data)
    {
        GE_CORE_CHECK(data["type"].get<std::string>() == GetType(),
                      "[TriangleMesh::Deserialize] Error input type '{}'",
                      data["type"].get<std::string>());
        if (data["type"].get<std::string>() == GetType())
        {
            m_alias          = data["alias"].get<std::string>();
            auto&& mesh_path = data["mesh"].get<std::string>();
            if (!mesh_path.empty())
            {
                m_meshResource = ResourceManager::GetResource<TriangleMeshResource>(mesh_path);
            }
        }
    }
} // namespace GE