#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Math.h"

#include "VulkanManager/VulkanCreateInfoBuilder.h"

#include "RenderPipeline.h"

#include "RenderResource.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    struct RenderPassRunData
    {
        uint                     frame_idx;
        VkCommandBuffer          cmd;
        std::vector<VkSemaphore> wait_semaphores;
        std::vector<VkSemaphore> signal_semaphores;
        VkFence                  fence;
        RenderResourceManager&   resource_manager;
    };

    class RenderPassBase
    {
    public:
        RenderPassBase(RenderResourceManager& resource_manager, std::string identifier_prefix) :
            m_resourceManager(resource_manager)
        {
            m_identifierPrefix = "RenderPass/" + identifier_prefix;
            if (!m_identifierPrefix.ends_with("/"))
            {
                m_identifierPrefix += "/";
            }
        }
        void Init(uint frame_cnt)
        {
            m_frameCnt        = frame_cnt;
            m_signalSemaphore = VulkanCore::CreateSemaphore();
            InitInternal(frame_cnt);
            BuildInternal();
        }

        virtual void Resize(uint2 size) {}

        inline VkSemaphore GetSignaledSemaphore() { return m_signalSemaphore; }
        inline std::string FullIdentifier(std::string suffix) { return m_identifierPrefix + suffix; }

    protected:
        virtual void InitInternal(uint frame_cnt) = 0; // Override by final class
        virtual void BuildInternal() = 0; // Override by different pass type

    protected:
        std::string            m_identifierPrefix = "";
        std::string            m_name             = "";
        uint                   m_frameCnt         = 1;
        bool                   m_ready            = false;
        VkSemaphore            m_signalSemaphore  = VK_NULL_HANDLE;
        RenderResourceManager& m_resourceManager;
    };

    class GraphicsPassResource
    {
    public:
        GraphicsPassResource()
        {
            desc   = VkInit::GetAttachmentDescription();
            blend  = VkInit::GetPipelineColorBlendAttachmentState();
            layout = VK_IMAGE_LAYOUT_GENERAL;
        }
        VkAttachmentDescription             desc;
        VkPipelineColorBlendAttachmentState blend;
        VkImageLayout                       layout;
    };

    /**
     * @brief warper for vulkan render pass, defaults to have only one subpass
     */
    class GraphicsPass : public RenderPassBase
    {
    public:
        GraphicsPass(RenderResourceManager& resource_manager, std::string identifier_prefix) :
            RenderPassBase(resource_manager, "Graphics/" + identifier_prefix)
        {}
        virtual ~GraphicsPass();

        virtual void Resize(uint width, uint height) { m_extent = {width, height}; };

        inline VkRenderPass     GetRenderPass() { return m_renderPass; }
        inline VkPipeline       GetPipeline() { return m_pipeline.GetPipeline(); }
        inline VkPipelineLayout GetPipelineLayout() { return m_pipeline.GetPipelineLayout(); }

    protected:
        virtual void BuildInternal() override;
        void __update_resource();

    public:
        std::vector<GraphicsPassResource> m_input, m_output;

    protected:
        VkRenderPass m_renderPass;

        bool m_ready = false;

        bool                    m_enableDepthStencil = false;
        VkAttachmentDescription m_depthAttachment;
        VkAttachmentReference   m_depthReference;

        std::vector<VkAttachmentDescription>             m_flattenAttachments;
        std::vector<VkPipelineColorBlendAttachmentState> m_flattenAttachmentBlendStates;
        std::vector<VkAttachmentReference>               m_inputRefference, m_outputReference;
        std::vector<VkSubpassDependency>                 m_dependencies;
        std::vector<VkSubpassDescription>                m_subpasses;

        GraphicsRenderPipeline m_pipeline;

        VkExtent2D m_extent = {1920, 1080};
    };

    class ComputePass : public RenderPassBase
    {
    public:
        ComputePass(RenderResourceManager& resource_manager, std::string identifier_prefix) :
            RenderPassBase(resource_manager, "Compute/" + identifier_prefix)
        {}
        virtual ~ComputePass();

        inline VkPipeline       GetPipeline() { return m_pipeline.GetPipeline(); }
        inline VkPipelineLayout GetPipelineLayout() { return m_pipeline.GetPipelineLayout(); }

    protected:
        virtual void BuildInternal() override;

    protected:
        ComputeRenderPipeline m_pipeline;
    };
} // namespace GE