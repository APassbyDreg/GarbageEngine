#include "TriangleMesh.h"

#include "../ShaderManager/HLSLCompiler.h"

#include "Runtime/function/Scene/Components/Transform.h"

#include "Runtime/resource/Managers/ResourceManager.h"
#include "Runtime/resource/ResourceTypes/TriangleMeshResource.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "Runtime/Application.h"

namespace GE
{
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

    void TriangleMesh::SetupRenderPipeline(GraphicsRenderPipeline& pipeline)
    {
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
        pipeline.AddDescriptorSetLayoutBinding(c_instanceDataDescriptorID,
                                               VkInit::GetDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                                                     VK_SHADER_STAGE_ALL_GRAPHICS,
                                                                                     c_meshInstanceDataBinding));
    }

    void TriangleMesh::SetupPassResources(GraphicsPassUnit& unit)
    {
        RenderResourceManager& resource_manager = unit.GetPass().GetResourceManager();
        {
            resource_manager.ReservePerFramePersistantDescriptorSet(
                unit.FullIdentifier("InstanceDataDescriptorSet"),
                unit.GetPipeline().GetDescriptorSetLayout(c_instanceDataDescriptorID));
            // buffer
            auto buffer_info =
                VkInit::GetBufferCreateInfo(sizeof(VertexInstanceData),
                                            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                                                VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            auto alloc_info = VkInit::GetAllocationCreateInfo(VMA_MEMORY_USAGE_AUTO,
                                                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
            resource_manager.ReservePerFrameDynamicBuffer(unit.FullIdentifier("InstanceData"), buffer_info, alloc_info);
        }
    }

    void TriangleMesh::RunRenderPass(MeshRenderPassData data)
    {
        Activate();

        auto&& [frame_idx, cmd, instances, unit] = data;

        // bind vertex buffer and index buffer
        {
            VkDeviceSize offsets[] = {0};
            VkBuffer     buffers[] = {m_vertexBuffer.GetBuffer()};
            vkCmdBindVertexBuffers(cmd, c_vertexBindId, 1, buffers, offsets);
            vkCmdBindIndexBuffer(cmd, m_indexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }

        // generate and upload instance data
        uint                            num_instance = instances.size();
        std::vector<VertexInstanceData> instance_data;
        instance_data.reserve(num_instance);
        for (auto&& instance : instances)
        {
            float4x4 model_mat       = instance->GetComponent<TransformComponent>().GetTransformMatrix();
            float4x4 t_inv_model_mat = glm::transpose(glm::inverse(model_mat));
            instance_data.push_back({model_mat, t_inv_model_mat});
        }
        auto&& resource_manager = unit.GetPass().GetResourceManager();
        auto&& instance_buffer =
            resource_manager.GetPerFrameDynamicBuffer(frame_idx, unit.FullIdentifier("InstanceData"));
        instance_buffer->UploadAs(instance_data);

        // write instance descriptor and dispatch
        VkDescriptorSet per_instance_desc_set = resource_manager.GetPerFramePersistantDescriptorSet(
            frame_idx, unit.FullIdentifier("InstanceDataDescriptorSet"));
        VkDescriptorBufferInfo instance_buffer_info = {};
        instance_buffer_info.buffer                 = instance_buffer->GetBuffer();
        instance_buffer_info.offset                 = 0;
        instance_buffer_info.range                  = sizeof(VertexInstanceData) * num_instance;
        VkWriteDescriptorSet write                  = {};
        write.sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet                                = per_instance_desc_set;
        write.descriptorType                        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write.descriptorCount                       = 1;
        write.dstBinding                            = c_meshInstanceDataBinding;
        write.dstArrayElement                       = 0;
        write.pBufferInfo                           = &instance_buffer_info;
        VulkanCore::WriteDescriptors({write});

        vkCmdBindDescriptorSets(cmd,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                unit.GetPipeline().GetPipelineLayout(),
                                c_instanceDataDescriptorID,
                                1,
                                &per_instance_desc_set,
                                0,
                                nullptr);

        vkCmdDrawIndexed(cmd, m_meshResource->GetIndexCount(), num_instance, 0, 0, 0);
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
            m_fileDialogInstance.OpenDialog(
                "ChooseFileDlgKey", "Choose File", ".ge.trianglemesh,.*", workdir.string().c_str());
        }
        if (m_fileDialogInstance.Display("ChooseFileDlgKey"))
        {
            if (m_fileDialogInstance.IsOk())
            {
                std::string filepath = m_fileDialogInstance.GetFilePathName();
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
            m_fileDialogInstance.Close();
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