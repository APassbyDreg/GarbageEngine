#include "RenderResource.h"

namespace GE
{
    void RenderResourceManager::Init(uint num_frames)
    {
        for (int i = 0; i < num_frames; i++)
        {
            ResourceBlock&& block = {};
            m_perFrameResources.emplace_back(block);
            m_perFrameResources[i].Init();
        }
        m_globalResources.Init();
        m_numFrames = num_frames;
    }

    void RenderResourceManager::NewFrame(uint frame_id) { m_perFrameResources[frame_id].Reset(); }
} // namespace GE