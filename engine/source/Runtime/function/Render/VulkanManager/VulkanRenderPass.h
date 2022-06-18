#pragma once

#include "GE_pch.h"

#include "VulkanCommon.h"

namespace GE
{
    class VulkanRenderPass
    {
        inline VkRenderPass GetRenderPass() { return m_renderPass; }

    private:
        VkRenderPass m_renderPass;
    };
} // namespace GE