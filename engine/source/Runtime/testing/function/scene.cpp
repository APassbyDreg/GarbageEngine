#include "scene.h"
#include "Runtime/function/Components/Transform.h"

namespace GE
{
    void SceneECSTestCase::run()
    {
        Scene sc  = {};
        auto& reg = sc.GetRegistry();

        std::shared_ptr<Entity> e = sc.CreateEntity();
        e->AddComponent<TransformComponent>(float3(0, 0, 1), float3(1, 2, 3), quat(1, 0, 0, 0));

        if (e->HasComponent<TransformComponent>())
        {
            TransformComponent& trans = e->GetComponent<TransformComponent>();
            GE_CORE_INFO("Transform Matrix: {}", trans.GetTransformMatrix());
            GE_CORE_INFO("Transform Component: {}", SerializeComponent<TransformComponent>(trans).dump());
        }
    }
} // namespace GE