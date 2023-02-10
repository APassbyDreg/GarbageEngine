#include "RenderResource.h"

namespace GE
{
    void RenderResourceManager::Init(uint num_frames)
    {
        for (int i = 0; i < num_frames; i++)
        {
            {
                ResourceBlock&& block = {};
                m_perFrameResources.emplace_back(block);
                m_perFrameResources[i].Init();
            }
            {
                ResourceBlock&& block = {};
                m_perFramePersistantResources.emplace_back(block);
                m_perFramePersistantResources[i].Init();
            }
        }
        m_globalResources.Init();
        m_globalPersistantResources.Init();
        m_numFrames = num_frames;
    }

    void RenderResourceManager::NewFrame(uint frame_id) { m_perFrameResources[frame_id].Reset(); }

    void RenderResourceManager::Reset()
    {
        m_globalResources.Reset();
        m_globalPersistantResources.Reset();

        for (auto&& frame_resource : m_perFrameResources)
        {
            frame_resource.Reset();
        }
        for (auto&& frame_resource : m_perFramePersistantResources)
        {
            frame_resource.Reset();
        }
    }
} // namespace GE