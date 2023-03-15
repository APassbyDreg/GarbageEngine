#include "ViewUniform.h"

#include "Runtime/function/Scene/Manager/CameraManager.h"

#include "Runtime/core/Math/Transforms.h"

namespace GE
{
    CameraUniform CameraUniform::FromCameraInfo(CameraInfo& info)
    {
        CameraUniform uniform = {};

        // space transforms
        uniform.world_to_view = info.v_matrix;
        uniform.view_to_clip  = info.p_matrix;
        uniform.world_to_clip = info.p_matrix * info.v_matrix;
        uniform.clip_to_view  = glm::inverse(uniform.view_to_clip);
        uniform.clip_to_world = glm::inverse(uniform.world_to_clip);

        // position
        uniform.pos = info.position;

        // bases
        float4x4 view_to_world = glm::inverse(info.v_matrix);
        uniform.forward        = glm::normalize(Math::HomogeneousTransform(float3(0, 0, 1), view_to_world));
        uniform.up             = glm::normalize(Math::HomogeneousTransform(float3(0, 1, 0), view_to_world));
        uniform.right          = glm::normalize(Math::HomogeneousTransform(float3(1, 0, 0), view_to_world));

        // clip
        uniform.clip     = info.clip;
        uniform.inv_clip = 1.0f / info.clip;

        return uniform;
    }

    VkDescriptorSetLayout ViewUniform::GetDescriptorSetLayout()
    {
        static std::shared_ptr<DescriptorSetLayout> layout = nullptr;
        if (layout == nullptr)
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            bindings.push_back(
                VkInit::GetDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL, 0));
            auto&& info = VkInit::GetDescriptorSetLayoutCreateInfo(bindings);
            layout      = std::make_shared<DescriptorSetLayout>(info);
        }
        return layout->Get();
    }
} // namespace GE