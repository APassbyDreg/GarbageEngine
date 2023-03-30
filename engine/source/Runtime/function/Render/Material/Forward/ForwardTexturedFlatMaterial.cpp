#pragma once

#include "ForwardTexturedFlatMaterial.h"

#include "Runtime/function/Render/RenderPass/ForwardShadingPass.h"
#include "Runtime/function/Render/ShaderManager/HLSLCompiler.h"
#include "Runtime/function/Render/VulkanManager/RenderUtils.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include "Runtime/Application.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    void ForwardTexturedFlatMaterial::SetupRenderPipeline(GraphicsRenderPipeline& pipeline)
    {
        // descriptor
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding                      = 0;
        binding.descriptorCount              = 1;
        binding.descriptorType               = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.stageFlags                   = VK_SHADER_STAGE_FRAGMENT_BIT;
        pipeline.AddDescriptorSetLayoutBinding(c_materialDataDescriptorID, binding);

        // shader
        fs::path     fspath     = fs::path(Config::shader_dir) / "Passes/Forward/TexturedFlat.gsf";
        HLSLCompiler fscompiler = {ShaderType::FRAGMENT};
        pipeline.AddShaderModule(fscompiler.Compile(fspath.string(), "FSMain"));

        // states
        pipeline.m_rasterizationState = VkInit::GetPipelineRasterizationStateCreateInfo();
    }

    void ForwardTexturedFlatMaterial::SetupPassResources(GraphicsPassUnit& pass)
    {
        auto&& renderpass = pass.GetPass();
        auto&& pipeline   = pass.GetPipeline();

        renderpass.GetResourceManager().ReserveGlobalPersistantDescriptorSet(
            std::format("Material/ForwardFlatMaterial-{}/Texture", m_id),
            pipeline.GetDescriptorSetLayout(c_materialDataDescriptorID));
    }

    void ForwardTexturedFlatMaterial::RunShadingPass(MaterialRenderPassData data)
    {
        auto&& [frame_id, cmd, unit] = data;
        auto&& pipeline              = unit.GetPipeline();

        auto descriptor = unit.GetPass().GetResourceManager().GetGlobalPersistantDescriptorSet(
            std::format("Material/ForwardFlatMaterial-{}/Texture", m_id));
        Activate(descriptor);

        vkCmdBindDescriptorSets(cmd,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeline.GetPipelineLayout(),
                                c_materialDataDescriptorID,
                                1,
                                &descriptor,
                                0,
                                nullptr);
    }

    bool ForwardTexturedFlatMaterial::IsOpaque() const { return true; }

    void ForwardTexturedFlatMaterial::Deserialize(const json& data)
    {
        GE_CORE_CHECK(data["type"].get<std::string>() == GetType(),
                      "[ForwardTexturedFlatMaterial::Deserialize] Error input type '{}'",
                      data["type"].get<std::string>());
        if (data["type"].get<std::string>() == GetType())
        {
            m_alias = data["alias"].get<std::string>();
            if (data.contains("image"))
            {
                m_textureResource = ResourceManager::GetResource<LDRImageResource>(data["image"].get<std::string>());
            }
        }
    }

    json ForwardTexturedFlatMaterial::Serialize() const
    {
        json data;
        if (m_textureResource != nullptr)
        {
            data["image"] = m_textureResource->GetFilePath().string();
        }
        return data;
    }

    void ForwardTexturedFlatMaterial::Inspect()
    {
        char buf[256];
        strcpy(buf, m_alias.c_str());
        ImGui::InputText("Alias", buf, 256);
        m_alias = buf;

        if (m_textureResource != nullptr)
        {
            ImGui::Text(m_textureResource->GetFilePath().string().c_str());
        }
        if (ImGui::Button("Select Texture"))
        {
            Application& app     = Application::GetInstance();
            fs::path     workdir = app.GetWorkDirectory();
            ImGuiFileDialog::Instance()->OpenDialog("ForwardTexturedFlatMaterial_SelectTexture",
                                                    "Choose Texture Image",
                                                    ".png,.jpg,.jpeg",
                                                    workdir.string());
        }

        if (ImGuiFileDialog::Instance()->Display("ForwardTexturedFlatMaterial_SelectTexture"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                m_textureResource        = ResourceManager::GetResource<LDRImageResource>(filePathName);
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }

    void ForwardTexturedFlatMaterial::Activate(VkDescriptorSet descriptor)
    {
        GE_CORE_ASSERT(m_textureResource != nullptr,
                       "[ForwardTexturedFlatMaterial::Activate] Texture resource is null");
        if (!m_texture->IsValid())
        {
            // create sampler
            {
                auto info = VkInit::GetSamplerCreateInfo(VK_FILTER_LINEAR, VK_FILTER_LINEAR);
                m_sampler = info;
            }

            // alloc image
            auto&& img = m_textureResource->GetData();
            {
                auto                    img_info   = img.GetImageCreateInfo();
                VmaAllocationCreateInfo alloc_info = {};
                alloc_info.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;
                alloc_info.requiredFlags           = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
                m_texture->Alloc(img_info, alloc_info);
            }

            // upload to staging buffer
            GpuBuffer stage_buffer;
            {
                size_t img_buffer_size = img.GetBufferSize();
                auto   alloc_info      = VkInit::GetAllocationCreateInfo(
                    VMA_MEMORY_USAGE_CPU_ONLY, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
                auto stage_info = VkInit::GetBufferCreateInfo(
                    img_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
                stage_buffer.Alloc(stage_info, alloc_info);
                stage_buffer.Upload(img.GetData(), img_buffer_size);
            }

            // do layout transitions and transfer buffer to image
            {
                auto&& ctx = RenderUtils::GetOneTimeTransferContext();
                ctx.Run([&](VkCommandBuffer cmd) {
                    // transform the image layout to transfer dst
                    RenderUtils::TransitionImageLayout(cmd,
                                                       m_texture->GetImage(),
                                                       VK_IMAGE_LAYOUT_UNDEFINED,
                                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                       RenderUtils::AllImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT),
                                                       0,
                                                       VK_ACCESS_TRANSFER_WRITE_BIT,
                                                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                                       VK_PIPELINE_STAGE_TRANSFER_BIT);

                    // copy the buffer into the image
                    VkBufferImageCopy copyRegion               = {};
                    copyRegion.bufferOffset                    = 0;
                    copyRegion.bufferRowLength                 = 0;
                    copyRegion.bufferImageHeight               = 0;
                    copyRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                    copyRegion.imageSubresource.mipLevel       = 0;
                    copyRegion.imageSubresource.baseArrayLayer = 0;
                    copyRegion.imageSubresource.layerCount     = 1;
                    copyRegion.imageExtent                     = m_texture->GetExtent();
                    vkCmdCopyBufferToImage(cmd,
                                           stage_buffer.GetBuffer(),
                                           m_texture->GetImage(),
                                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                           1,
                                           &copyRegion);

                    // transform the image layout to shader read
                    RenderUtils::TransitionImageLayout(cmd,
                                                       m_texture->GetImage(),
                                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                       RenderUtils::AllImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT),
                                                       0,
                                                       VK_ACCESS_TRANSFER_WRITE_BIT,
                                                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                                       VK_PIPELINE_STAGE_TRANSFER_BIT);
                });
            }

            // bind to descriptor
            {
                auto view_info = VkInit::GetVkImageViewCreateInfo(m_texture->GetImageInfo(), VK_IMAGE_ASPECT_COLOR_BIT);

                VkDescriptorImageInfo image_info = {};
                image_info.imageLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                image_info.imageView             = m_texture->GetImageView(view_info, true);
                image_info.sampler               = m_sampler;

                VkWriteDescriptorSet write = {};
                write.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet               = descriptor;
                write.dstBinding           = 0;
                write.descriptorCount      = 1;
                write.descriptorType       = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write.pImageInfo           = &image_info;

                VulkanCore::WriteDescriptors({write});
            }
        }
    }

    void ForwardTexturedFlatMaterial::Deactivate()
    {
        m_texture->Delete();
        m_sampler.Destroy();
    }
} // namespace GE