#include "scene.h"

#include "Runtime/function/Scene/Components/ComponentFactory.h"


namespace GE
{
    void SceneECSTestCase::run()
    {
        Scene sc = {};
        auto  e0 = sc.CreateEntity("Entity 0", 0, 0);
        e0->AddComponent<TransformComponent>(float3(0, 0, 1), float3(1, 2, 3), quat(1, 0, 0, 0));
        if (e0->HasComponent<TransformComponent>())
        {
            TransformComponent& trans = e0->GetComponent<TransformComponent>();
        }
        auto e1 = sc.CreateEntity("Entity 1", 1, 0);
        e1->RemoveComponent<TagComponent>();

        json data = sc.Serialize();
        GE_CORE_INFO("Serialized scene: {}", data.dump());
    }
} // namespace GE