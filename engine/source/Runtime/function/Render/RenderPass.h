#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Math.h"

#include "VulkanManager/VulkanCreateInfoBuilder.h"

#include "RenderPipeline.h"

#include "RenderResource.h"

namespace GE
{
    struct RenderPassRunData
    {
        uint            frame_idx = std::numeric_limits<uint>::max();
        VkCommandBuffer cmd       = VK_NULL_HANDLE;
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
        virtual ~RenderPassBase() = default;

        virtual void Init(uint frame_cnt)            = 0;
        virtual void Resize(uint width, uint height) = 0;

        inline std::string            FullIdentifier(std::string suffix = "") { return m_identifierPrefix + suffix; }
        inline RenderResourceManager& GetResourceManager() { return m_resourceManager; }

    protected:
        std::string            m_identifierPrefix = "";
        std::string            m_name             = "";
        uint                   m_frameCnt         = 1;
        RenderResourceManager& m_resourceManager;
    };

    class GraphicsColorResource
    {
    public:
        GraphicsColorResource()
        {
            desc   = VkInit::GetAttachmentDescription();
            blend  = VkInit::GetPipelineColorBlendAttachmentState();
            layout = VK_IMAGE_LAYOUT_GENERAL;
        }
        VkAttachmentDescription             desc;
        VkPipelineColorBlendAttachmentState blend;
        VkImageLayout                       layout;
    };

    class RenderPass
    {
    public:
        ~RenderPass()
        {
            if (m_built && m_renderPass != VK_NULL_HANDLE)
            {
                vkDestroyRenderPass(VulkanCore::GetDevice(), m_renderPass, nullptr);
            }
        }

        std::vector<GraphicsColorResource> input, output;

        bool                    enableDepthStencil = false;
        VkAttachmentDescription depthAttachment;

        void Build();

        inline VkRenderPass GetRenderPass()
        {
            GE_CORE_CHECK(m_built, "Trying to get an unintialized RenderPass");
            return m_renderPass;
        }

        inline bool IsBuilt() const { return m_built; }

        inline const std::vector<VkPipelineColorBlendAttachmentState>& GetFlattenColorBlendStates() const
        {
            GE_CORE_ASSERT(m_built, "Trying to get ColorBlendAttachmentState from an unintialized RenderPass");
            return m_flattenAttachmentBlendStates;
        }

    private:
        std::vector<VkAttachmentDescription>             m_flattenAttachments;
        std::vector<VkPipelineColorBlendAttachmentState> m_flattenAttachmentBlendStates;
        std::vector<VkAttachmentReference>               m_inputRefference, m_outputReference;
        VkAttachmentReference                            m_depthReference;

        std::vector<VkSubpassDependency>  m_dependencies;
        std::vector<VkSubpassDescription> m_subpasses;

        bool         m_built      = false;
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
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
        virtual ~GraphicsPass() {};

        virtual void Resize(uint width, uint height) = 0;

        inline VkRenderPass GetRenderPass() { return m_renderPass.GetRenderPass(); }
        inline RenderPass&  GetRenderPassObject() { return m_renderPass; }
        inline VkExtent2D   GetExtent() const { return m_extent; }

    protected:
        VkExtent2D                                m_extent = {1920, 1080};
        RenderPass                                m_renderPass;
        std::vector<std::shared_ptr<FrameBuffer>> m_frameBuffers;
    };

    class GraphicsPassUnit
    {
    public:
        GraphicsPassUnit(GraphicsPass& pass) : m_pass(pass) {}

        inline GraphicsRenderPipeline& GetPipeline() { return *m_pipeline; }
        inline GraphicsPass&           GetPass() { return m_pass; }

        virtual std::string FullIdentifier(std::string identifier) = 0;

    protected:
        std::shared_ptr<GraphicsRenderPipeline> m_pipeline;
        GraphicsPass&                           m_pass;
    };

    class ComputePass : public RenderPassBase
    {
    public:
        ComputePass(RenderResourceManager& resource_manager, std::string identifier_prefix) :
            RenderPassBase(resource_manager, "Compute/" + identifier_prefix)
        {}
        virtual ~ComputePass() {};

        inline VkPipeline             GetPipeline() { return m_pipeline->GetPipeline(); }
        inline VkPipelineLayout       GetPipelineLayout() { return m_pipeline->GetPipelineLayout(); }
        inline ComputeRenderPipeline& GetPipelineObject() { return *m_pipeline; }

    protected:
        std::shared_ptr<ComputeRenderPipeline> m_pipeline;
    };
} // namespace GE