#include "BuiltinRenderRoutine.h"

#include "Runtime/function/Scene/Components/Material.h"
#include "Runtime/function/Scene/Components/Mesh.h"
#include "Runtime/function/Scene/Scene.h"

#include "Runtime/core/Utils/ContainerUtils.h"

#include "Runtime/Application.h"

#include "../VulkanManager/RenderUtils.h"
#include "vulkan/vulkan_core.h"
#include <functional>
#include <memory>

namespace GE
{
    BuiltinRenderRoutine::BuiltinRenderRoutine() {}

    BuiltinRenderRoutine::BuiltinRenderRoutine(uint n_frames) { Init(n_frames); }

    BuiltinRenderRoutine::~BuiltinRenderRoutine() {}

    void BuiltinRenderRoutine::Resize(uint width, uint height)
    {
        if (width == 0 || height == 0)
        {
            GE_CORE_ERROR("[BuiltinRenderRoutine::Resize] Invalid width or height");
            return;
        }
        if (width != m_viewportSize.width || height != m_viewportSize.height)
        {
            m_viewportSize.width  = width;
            m_viewportSize.height = height;

            // wait all old frames to finish
            vkQueueWaitIdle(VulkanCore::GetGraphicsQueue());

            // recreate frame image
            auto&&                  color_images = m_renderResourceManager.GetFramewiseImage("ColorRT");
            auto&&                  depth_images = m_renderResourceManager.GetFramewiseImage("DepthRT");
            VmaAllocationCreateInfo alloc_info   = {};
            alloc_info.usage                     = VMA_MEMORY_USAGE_GPU_ONLY;
            alloc_info.requiredFlags             = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            VkImageUsageFlags color_image_usage =
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            VkImageCreateInfo color_image_info = VkInit::GetVkImageCreateInfo(
                VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, {width, height, 1}, color_image_usage);
            VkImageViewCreateInfo color_view_info =
                VkInit::GetVkImageViewCreateInfo(color_image_info, VK_IMAGE_ASPECT_COLOR_BIT);
            VkImageUsageFlags depth_image_usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                                  VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            VkImageCreateInfo depth_image_info = VkInit::GetVkImageCreateInfo(
                VK_IMAGE_TYPE_2D, VK_FORMAT_D32_SFLOAT, {width, height, 1}, depth_image_usage);
            VkImageViewCreateInfo depth_view_info =
                VkInit::GetVkImageViewCreateInfo(depth_image_info, VK_IMAGE_ASPECT_DEPTH_BIT);
            for (size_t i = 0; i < m_frameCnt; i++)
            {
                color_images[i]->Delete();
                color_images[i]->Alloc(color_image_info, color_view_info, alloc_info);
                depth_images[i]->Delete();
                depth_images[i]->Alloc(depth_image_info, depth_view_info, alloc_info);
            }

            // resize all passes
            m_forwardPass.Resize(width, height);
        }
    }

    void BuiltinRenderRoutine::Init(uint n_frames)
    {
        GE_CORE_ASSERT(n_frames > 0, "[BuiltinRenderRoutine::Init] At least one frame is needed.");
        m_tLastFrame = m_tInit = Time::CurrentTime();

        /* --------------------------- basic info --------------------------- */
        m_frameCnt = n_frames;
        m_renderResourceManager.Init(n_frames);

        /* ------------------------- render targets ------------------------- */
        m_renderResourceManager.ReservePerFrameImage("ColorRT");
        m_renderResourceManager.ReservePerFrameImage("DepthRT");

        /* -------------------------- initial size -------------------------- */
        Resize(1280, 720);

        /* -------------------------- view uniform -------------------------- */
        {
            // buffer
            VmaAllocationCreateInfo alloc_info = VkInit::GetAllocationCreateInfo(
                VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
            VkBufferCreateInfo buffer_info =
                VkInit::GetBufferCreateInfo(sizeof(ViewUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
            m_renderResourceManager.ReservePerFrameStaticBuffer("ViewUniform/MainCamera", buffer_info, alloc_info);

            // set
            std::vector<VkDescriptorSetLayout> layouts  = {ViewUniform::GetDescriptorSetLayout()};
            VkDescriptorSetAllocateInfo        set_info = VkInit::GetDescriptorSetAllocateInfo(layouts);
            m_renderResourceManager.ReservePerFramePersistantDescriptorSet("ViewUniform/MainCamera", set_info);

            // write to set
            auto&& buffers     = m_renderResourceManager.GetFramewiseStaticBuffer("ViewUniform/MainCamera");
            auto&& descriptors = m_renderResourceManager.GetFramewisePersistantDescriptorSet("ViewUniform/MainCamera");
            std::vector<VkWriteDescriptorSet> writes = {};
            for (int frame = 0; frame < n_frames; frame++)
            {
                VkDescriptorBufferInfo buffer_info = {};
                buffer_info.buffer                 = buffers[frame]->GetBuffer();
                buffer_info.offset                 = 0;
                buffer_info.range                  = VK_WHOLE_SIZE;
                VkWriteDescriptorSet write         = {};
                write.sType                        = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet                       = descriptors[frame];
                write.descriptorType               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.descriptorCount              = 1;
                write.dstBinding                   = 0;
                write.dstArrayElement              = 0;
                write.pBufferInfo                  = &buffer_info;
                writes.push_back(write);
            }
            VulkanCore::WriteDescriptors(writes);
        }

        /* ------------------------- command buffer ------------------------- */
        m_renderResourceManager.ReservePerFrameGraphicsCmdBuffer("Main");

        /* ------------------------- control values ------------------------- */
        m_renderResourceManager.ReservePerFrameSemaphore("RenderTargetTransition");
        m_renderResourceManager.ReservePerFrameSemaphore("ForwardPass");

        /* ----------------------- init render passes ----------------------- */
        m_forwardPass.Init(n_frames);
    }

    void BuiltinRenderRoutine::DrawFrame(uint                     index,
                                         std::vector<VkSemaphore> routine_wait_semaphores,
                                         std::vector<VkSemaphore> routine_signal_semaphores,
                                         VkFence                  fence)
    {
        // initialize frame
        uint frame_index = index % m_frameCnt;
        m_renderResourceManager.NewFrame(index % m_frameCnt);
        Time::TimeStamp t                 = Time::CurrentTime();
        ViewUniform     base_view_uniform = GetBaseViewUniform(t);

        // initialize cmd buffer
        auto&& cmd = m_renderResourceManager.GetPerFrameGraphicsCmdBuffer(frame_index, "Main");
        RenderUtils::BeginOneTimeSubmitCmdBuffer(cmd);

        // initialize view info
        auto&& sc          = Application::GetInstance().GetActiveScene();
        auto&& cam_manager = sc->GetCameraManager();
        m_prevCamInfo      = m_currCamInfo;
        m_currCamInfo      = cam_manager.GetActiveCameraInfo(m_viewportSize.width / (float)m_viewportSize.height);
        ViewUniform cam_view_uniform    = base_view_uniform;
        cam_view_uniform.curr_cam       = CameraUniform::FromCameraInfo(m_currCamInfo);
        cam_view_uniform.prev_cam       = CameraUniform::FromCameraInfo(m_prevCamInfo);
        cam_view_uniform.has_prev_frame = uint(m_currCamInfo.eid != m_prevCamInfo.eid);

        // update camera uniform buffer
        auto&& cam_view_uniform_buffer =
            m_renderResourceManager.GetPerFrameStaticBuffer(frame_index, "ViewUniform/MainCamera");
        cam_view_uniform_buffer->Upload((byte*)&cam_view_uniform, sizeof(ViewUniform));

        // create and group renderables
        auto&& renderables        = sc->GetMeshManager().FrustumCull(cam_view_uniform.curr_cam.world_to_clip);
        using MeshMaterialPairKey = std::tuple<Mesh*, Material*>;
        std::map<MeshMaterialPairKey, std::vector<std::shared_ptr<Entity>>> renderable_groups;
        for (auto&& r : renderables)
        {
            auto mat_id  = r->GetComponent<MaterialComponent>().GetCoreValue().get();
            auto mesh_id = r->GetComponent<MeshComponent>().GetCoreValue().get();
            renderable_groups[{mesh_id, mat_id}].push_back(r);
        }
        auto forward_renderables  = StdUtils::FilterKey(renderable_groups, [&](const MeshMaterialPairKey& key) {
            return renderable_groups[key][0]->GetComponent<MaterialComponent>().GetCoreValue()->GetMode() == "forward";
        });
        auto deferred_renderables = StdUtils::FilterKey(renderable_groups, [&](const MeshMaterialPairKey& key) {
            return renderable_groups[key][0]->GetComponent<MaterialComponent>().GetCoreValue()->GetMode() == "deferred";
        });

        // transition render target
        std::vector<float> clear_color = sc->GetSetting("TestSceneSetting")["Clear Color"].get<std::vector<float>>();
        {
            {
                VkImage           rt = m_renderResourceManager.GetPerFrameImage(frame_index, "ColorRT")->GetImage();
                VkClearColorValue clear_value  = {clear_color[0], clear_color[1], clear_color[2], 1.0f};
                VkImageSubresourceRange ranges = RenderUtils::AllImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
                RenderUtils::TransitionImageLayout(cmd, rt, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, ranges);
                vkCmdClearColorImage(cmd, rt, VK_IMAGE_LAYOUT_GENERAL, &clear_value, 1u, &ranges);
            }
            {
                VkImage rt = m_renderResourceManager.GetPerFrameImage(frame_index, "DepthRT")->GetImage();
                VkClearDepthStencilValue clear_value = {1.0f, 0};
                VkImageSubresourceRange  ranges      = RenderUtils::AllImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT);
                RenderUtils::TransitionImageLayout(cmd, rt, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, ranges);
                vkCmdClearDepthStencilImage(cmd, rt, VK_IMAGE_LAYOUT_GENERAL, &clear_value, 1u, &ranges);
            }
        }

        // forward pass
        bool need_forward_pass = forward_renderables.size() > 0;
        if (need_forward_pass)
        {
            std::vector<VkSemaphore> wait_semaphores = {
                m_renderResourceManager.GetPerFrameSemaphore(frame_index, "RenderTargetTransition")->Get()};
            std::vector<VkSemaphore> signal_semaphores = {
                m_renderResourceManager.GetPerFrameSemaphore(frame_index, "ForwardPass")->Get()};
            RenderPassRunData              run_data  = {frame_index, cmd};
            CombinedForwardShadingPassData pass_data = {forward_renderables};
            m_forwardPass.Run(run_data, pass_data);
        }

        // transition output
        {
            auto output = m_renderResourceManager.GetPerFrameImage(frame_index, "ColorRT");
            RenderUtils::TransitionImageLayout(cmd,
                                               output->GetImage(),
                                               VK_IMAGE_LAYOUT_GENERAL,
                                               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                               RenderUtils::AllImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT));
        }

        // submit
        RenderUtils::EndCmdBuffer(cmd);
        {
            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo         info       = {};
            info.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount         = routine_wait_semaphores.size();
            info.pWaitSemaphores            = routine_wait_semaphores.data();
            info.pWaitDstStageMask          = &wait_stage;
            info.commandBufferCount         = 1;
            info.pCommandBuffers            = &cmd;
            info.signalSemaphoreCount       = routine_signal_semaphores.size();
            info.pSignalSemaphores          = routine_signal_semaphores.data();
            VulkanCore::SubmitToGraphicsQueue(info, fence);
        }

        // finalize frame
        m_tLastFrame = t;
        m_frameRendered++;
    }

    ViewUniform BuiltinRenderRoutine::GetBaseViewUniform(Time::TimeStamp t)
    {
        ViewUniform data  = {};
        data.debug_flag   = 0;
        data.frame_number = m_frameRendered;
        data.random       = m_rand.RandInt<uint>();
        data.time         = Time::ToSeconds(t - m_tInit);
        data.delta_time   = Time::ToSeconds(t - m_tLastFrame);
        return data;
    }

    VkImageView BuiltinRenderRoutine::GetOutputImageView(uint frame_idx)
    {
        return m_renderResourceManager.GetPerFrameImage(frame_idx, "ColorRT")->GetImageView();
    }
} // namespace GE