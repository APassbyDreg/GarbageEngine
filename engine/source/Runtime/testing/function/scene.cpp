#include "scene.h"
#include "Runtime/function/Scene/Components/ComponentFactory.h"

namespace GE
{
    void SceneECSTestCase::run()
    {
        Scene sc  = {};
        auto e0 = sc.CreateEntity("Entity 0", 0, 0);
        e0->AddComponent<TransformComponent>(float3(0, 0, 1), float3(1, 2, 3), quat(1, 0, 0, 0));
        if (e0->HasComponent<TransformComponent>())
        {
            TransformComponent& trans = e0->GetComponent<TransformComponent>();
            GE_CORE_INFO("Transform Matrix: {}", trans.GetTransformMatrix());
            GE_CORE_INFO("Transform Component: {}", SerializeComponent<TransformComponent>(trans).dump());
        }
        auto e1 = sc.CreateEntity("Entity 1", 1, 0);
        e1->AddComponent<CameraComponent>();


        json data = sc.Serialize();
        GE_CORE_INFO("serialized scene: {}", data.dump());


        Scene sc_ = {};
        sc_.Deserialize(data);
        GE_CORE_INFO("scene after a loop: {}", sc_.Serialize().dump());
    }
} // namespace GE