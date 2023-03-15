#include "LightManager.h"

#include "Runtime/core/Utils/ContainerUtils.h"

#include "../Scene.h"

#include "../Components/Light.h"
#include "../Components/Transform.h"
#include <vector>

namespace GE
{
#define BIND_CLASS_FN(fn) std::bind(&SceneLightManager::fn, this, std::placeholders::_1)

    static bool IsManagable(Entity& e)
    {
        return e.HasComponent<TransformComponent>() && e.HasComponent<LightComponent>();
    }

    LightUniform SceneLightManager::GetLightUniformData()
    {
        LightUniform data;
        data.num_point_lights      = m_lightLists[LIGHT_TYPE_POINT].size();
        data.num_spot_lights       = m_lightLists[LIGHT_TYPE_SPOT].size();
        data.num_direction_lights  = m_lightLists[LIGHT_TYPE_DIRECTION].size();
        data.num_projection_lights = 0; // TODO: Support Projection Lights
        return data;
    }

    std::vector<PointLightData> SceneLightManager::GetPointLightData()
    {
        std::vector<PointLightData> data = {};
        for (auto&& [eid, entity] : m_lightLists[LIGHT_TYPE_POINT])
        {
            PointLightData light;
            auto&&         light_comp     = entity->GetComponent<LightComponent>().GetCoreValue();
            auto&&         transform_comp = entity->GetComponent<TransformComponent>().GetCoreValue();
            light.intensity               = light_comp.intensity * light_comp.color;
            light.position                = transform_comp.position;
            light.falloff_range           = light_comp.effect_range;
            light.radius                  = light_comp.radius;
            data.push_back(light);
        }
        return data;
    }

    std::vector<SpotLightData> SceneLightManager::GetSpotLightData()
    {
        std::vector<SpotLightData> data = {};
        for (auto&& [eid, entity] : m_lightLists[LIGHT_TYPE_SPOT])
        {
            SpotLightData light;
            auto&&        light_comp     = entity->GetComponent<LightComponent>().GetCoreValue();
            auto&&        transform_comp = entity->GetComponent<TransformComponent>().GetCoreValue();
            light.intensity              = light_comp.intensity * light_comp.color;
            light.position               = transform_comp.position;
            light.falloff_range          = light_comp.effect_range;
            light.direction              = transform_comp.Forward();
            light.up                     = transform_comp.Up();
            light.inner_angle            = light_comp.spread; // TODO: add new control parameter
            light.outer_angle            = light_comp.spread;
            light.aspect                 = light_comp.aspect;
            data.push_back(light);
        }
        return data;
    }

    std::vector<DirectionLightData> SceneLightManager::GetDirectionLightData()
    {
        std::vector<DirectionLightData> data = {};
        for (auto&& [eid, entity] : m_lightLists[LIGHT_TYPE_DIRECTION])
        {
            DirectionLightData light;
            auto&&             light_comp     = entity->GetComponent<LightComponent>().GetCoreValue();
            auto&&             transform_comp = entity->GetComponent<TransformComponent>().GetCoreValue();
            light.intensity                   = light_comp.intensity * light_comp.color;
            light.direction                   = transform_comp.Forward();
            data.push_back(light);
        }
        return data;
    }

    void SceneLightManager::Setup()
    {
        auto scene_id = m_scene.GetID();
        ComponentHook<TransformComponent>::AddChangedHook(BIND_CLASS_FN(UpdateEntity), scene_id);
        ComponentHook<LightComponent>::AddChangedHook(BIND_CLASS_FN(UpdateEntity), scene_id);

        ComponentHook<TransformComponent>::AddConstructHook(BIND_CLASS_FN(AddEntity), scene_id);
        ComponentHook<LightComponent>::AddConstructHook(BIND_CLASS_FN(AddEntity), scene_id);

        ComponentHook<TransformComponent>::AddDestructHook(BIND_CLASS_FN(RemoveEntity), scene_id);
        ComponentHook<LightComponent>::AddDestructHook(BIND_CLASS_FN(RemoveEntity), scene_id);
    }

    void SceneLightManager::Destroy() { m_lightLists.clear(); }

    void SceneLightManager::AddEntity(Entity& entity)
    {
        if (!IsManagable(entity))
        {
            return;
        }

        LightType type = entity.GetComponent<LightComponent>().GetCoreValue().type;
        auto      eid  = entity.GetEntityID();
        if (m_lightLists.find(type) == m_lightLists.end())
        {
            m_lightLists[type] = {};
        }

        if (m_lightLists[type].find(eid) != m_lightLists[type].end())
        {
            return;
        }

        m_lightLists[type][eid] = entity.shared_from_this();
        m_changed               = true;
    }

    void SceneLightManager::RemoveEntity(Entity& entity)
    {
        if (!IsManagable(entity))
        {
            return;
        }

        LightType type = entity.GetComponent<LightComponent>().GetCoreValue().type;
        auto      eid  = entity.GetEntityID();

        if (m_lightLists.find(type) == m_lightLists.end())
        {
            return;
        }

        m_lightLists[type].erase(eid);
        m_changed = true;
    }

    void SceneLightManager::UpdateEntity(Entity& entity) { m_changed = true; }

#undef BIND_CLASS_FN
} // namespace GE