#pragma once

#include "GE_pch.h"

#include "Runtime/core/Base/Singleton.h"
#include "Runtime/core/Json.h"

#include "VulkanManager/AutoGpuBuffer.h"
#include "VulkanManager/CommandPool.h"
#include "VulkanManager/DescriptorPool.h"
#include "VulkanManager/FrameBuffer.h"
#include "VulkanManager/GpuImage.h"
#include "VulkanManager/Synchronization.h"
#include "VulkanManager/VulkanCore.h"
#include <vector>

namespace GE
{
    class ResourceBlock
    {
#define IMPLEMENT_RENDER_RESOURCE_TYPE(name, type, initializer) \
public: \
    template<typename... TArgs> \
    inline type& Reserve##name(std::string identifier, TArgs&&... args) \
    { \
        if (m_##name##s.find(identifier) == m_##name##s.end()) \
            m_##name##s[identifier] = initializer; \
        return m_##name##s[identifier]; \
    } \
    inline type& Get##name(std::string identifier) \
    { \
        GE_CORE_ASSERT(m_##name##s.find(identifier) != m_##name##s.end(), \
                       "Trying to get a non-exist <" #name "> resource with identifier {}", \
                       identifier); \
        return m_##name##s[identifier]; \
    } \
    inline void RegisterExternal##name(std::string identifier, type resource) \
    { \
        GE_CORE_CHECK(m_##name##s.find(identifier) == m_##name##s.end(), \
                      "Trying to register an exsisting <" #name "> resource with identifier {}", \
                      identifier); \
        m_##name##s[identifier] = resource; \
    } \
\
private: \
    std::map<std::string, type> m_##name##s;

    public:
        inline void Init()
        {
            {
                VkDescriptorPoolSize       sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                                      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                                      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                                      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                                      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                                      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                                      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                                      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                                      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                                      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                                      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
                VkDescriptorPoolCreateInfo info    = {};
                info.sType                         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                info.flags                         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
                info.maxSets                       = 1000 * 11;
                info.poolSizeCount                 = 11;
                info.pPoolSizes                    = sizes;
                m_descPool                         = std::make_shared<DescriptorPool>(info);
            }
            {
                m_graphicsCmdPool = std::make_shared<CommandPool>(
                    VkInit::GetCommandPoolCreateInfo(VulkanCore::GetGraphicsQueueFamilyIndex()));
                m_computeCmdPool = std::make_shared<CommandPool>(
                    VkInit::GetCommandPoolCreateInfo(VulkanCore::GetComputeQueueFamilyIndex()));
            }
        }

        inline void Reset()
        {
            VulkanCore::ResetDescriptorPool(*m_descPool);
            VulkanCore::ResetCmdPool(*m_graphicsCmdPool);
            VulkanCore::ResetCmdPool(*m_computeCmdPool);
        }

        IMPLEMENT_RENDER_RESOURCE_TYPE(StaticBuffer,
                                       std::shared_ptr<GpuBuffer>,
                                       std::make_shared<GpuBuffer>(std::forward<TArgs>(args)...));
        IMPLEMENT_RENDER_RESOURCE_TYPE(DynamicBuffer,
                                       std::shared_ptr<AutoGpuBuffer>,
                                       std::make_shared<AutoGpuBuffer>(std::forward<TArgs>(args)...));
        IMPLEMENT_RENDER_RESOURCE_TYPE(Image,
                                       std::shared_ptr<GpuImage>,
                                       std::make_shared<GpuImage>(std::forward<TArgs>(args)...));
        IMPLEMENT_RENDER_RESOURCE_TYPE(Semaphore,
                                       std::shared_ptr<Semaphore>,
                                       std::make_shared<Semaphore>(std::forward<TArgs>(args)...));
        IMPLEMENT_RENDER_RESOURCE_TYPE(Fence,
                                       std::shared_ptr<Fence>,
                                       std::make_shared<Fence>(std::forward<TArgs>(args)...));
        IMPLEMENT_RENDER_RESOURCE_TYPE(DescriptorSet,
                                       VkDescriptorSet,
                                       VulkanCore::AllocDescriptorSet(*m_descPool, std::forward<TArgs>(args)...));
        IMPLEMENT_RENDER_RESOURCE_TYPE(GraphicsCmdBuffer,
                                       VkCommandBuffer,
                                       VulkanCore::CreateCmdBuffers(*m_graphicsCmdPool,
                                                                    1,
                                                                    std::forward<TArgs>(args)...)[0]);
        IMPLEMENT_RENDER_RESOURCE_TYPE(ComputeCmdBuffer,
                                       VkCommandBuffer,
                                       VulkanCore::CreateCmdBuffers(*m_computeCmdPool,
                                                                    1,
                                                                    std::forward<TArgs>(args)...)[0]);

    private:
        std::shared_ptr<DescriptorPool> m_descPool;
        std::shared_ptr<CommandPool>    m_graphicsCmdPool, m_computeCmdPool;

#undef IMPLEMENT_RENDER_RESOURCE_TYPE
    };

    class RenderResourceManager
    {
#define IMPLEMENT_RENDER_RESOURCE_TYPE(name, type) \
public: \
    inline type& GetPerFrame##name(uint frame, std::string identifier) \
    { \
        return m_perFrameResources[frame].Get##name(identifier); \
    } \
    inline std::vector<type> GetFramewise##name(std::string identifier) \
    { \
        std::vector<type> resources = {}; \
        for (auto&& frame : m_perFrameResources) \
        { \
            resources.emplace_back(frame.Get##name(identifier)); \
        } \
        return resources; \
    } \
    inline type& GetGlobal##name(std::string identifier) \
    { \
        return m_globalResources.Get##name(identifier); \
    } \
    template<typename... TArgs> \
    inline void ReservePerFrame##name(std::string identifier, TArgs&&... args) \
    { \
        for (int frame = 0; frame < m_numFrames; frame++) \
            m_perFrameResources[frame].Reserve##name(identifier, std::forward<TArgs>(args)...); \
    } \
    template<typename... TArgs> \
    inline void ReserveGlobal##name(std::string identifier, TArgs&&... args) \
    { \
        m_globalResources.Reserve##name(identifier, std::forward<TArgs>(args)...); \
    } \
    inline void RegisterExternalPerFrame##name(std::string identifier, std::vector<type> resources) \
    { \
        for (int frame = 0; frame < m_numFrames; frame++) \
            m_perFrameResources[frame].RegisterExternal##name(identifier, resources[frame]); \
    } \
    inline void RegisterExternalGlobal##name(std::string identifier, type resources) \
    { \
        m_globalResources.RegisterExternal##name(identifier, resources); \
    } \
    /* ------------------------- persistant variants ------------------------ */ \
    inline type& GetPerFramePersistant##name(uint frame, std::string identifier) \
    { \
        return m_perFramePersistantResources[frame].Get##name(identifier); \
    } \
    inline std::vector<type> GetFramewisePersistant##name(std::string identifier) \
    { \
        std::vector<type> resources = {}; \
        for (auto&& frame : m_perFramePersistantResources) \
        { \
            resources.emplace_back(frame.Get##name(identifier)); \
        } \
        return resources; \
    } \
    inline type& GetGlobalPersistant##name(std::string identifier) \
    { \
        return m_globalPersistantResources.Get##name(identifier); \
    } \
    template<typename... TArgs> \
    inline void ReservePerFramePersistant##name(std::string identifier, TArgs&&... args) \
    { \
        for (int frame = 0; frame < m_numFrames; frame++) \
            m_perFramePersistantResources[frame].Reserve##name(identifier, std::forward<TArgs>(args)...); \
    } \
    template<typename... TArgs> \
    inline void ReserveGlobalPersistant##name(std::string identifier, TArgs&&... args) \
    { \
        m_globalPersistantResources.Reserve##name(identifier, std::forward<TArgs>(args)...); \
    } \
    inline void RegisterExternalPerFramePersistant##name(std::string identifier, std::vector<type> resources) \
    { \
        for (int frame = 0; frame < m_numFrames; frame++) \
            m_perFramePersistantResources[frame].RegisterExternal##name(identifier, resources[frame]); \
    } \
    inline void RegisterExternalGlobalPersistant##name(std::string identifier, type resources) \
    { \
        m_globalPersistantResources.RegisterExternal##name(identifier, resources); \
    }

    public:
        // used to store some intermediate states for render pass
        inline void  SetState(std::string identifier, json state) { m_recordedStates[identifier] = state; }
        inline json& GetState(std::string identifier) { return m_recordedStates[identifier]; }
        template<class T>
        inline T GetState(std::string identifier)
        {
            return m_recordedStates[identifier].get<T>();
        }

        void Init(uint num_frames);
        void NewFrame(uint frame_id);
        void Reset();

        IMPLEMENT_RENDER_RESOURCE_TYPE(StaticBuffer, std::shared_ptr<GpuBuffer>);
        IMPLEMENT_RENDER_RESOURCE_TYPE(DynamicBuffer, std::shared_ptr<AutoGpuBuffer>);
        IMPLEMENT_RENDER_RESOURCE_TYPE(Image, std::shared_ptr<GpuImage>);
        IMPLEMENT_RENDER_RESOURCE_TYPE(Semaphore, std::shared_ptr<Semaphore>);
        IMPLEMENT_RENDER_RESOURCE_TYPE(Fence, std::shared_ptr<Fence>);
        IMPLEMENT_RENDER_RESOURCE_TYPE(DescriptorSet, VkDescriptorSet);
        IMPLEMENT_RENDER_RESOURCE_TYPE(GraphicsCmdBuffer, VkCommandBuffer);
        IMPLEMENT_RENDER_RESOURCE_TYPE(ComputeCmdBuffer, VkCommandBuffer);

    private:
        std::vector<ResourceBlock> m_perFrameResources = {};
        ResourceBlock              m_globalResources   = {};

        std::vector<ResourceBlock> m_perFramePersistantResources = {};
        ResourceBlock              m_globalPersistantResources   = {};

        json m_recordedStates;

        uint m_numFrames = 0;
#undef IMPLEMENT_RENDER_RESOURCE_TYPE
    };
} // namespace GE