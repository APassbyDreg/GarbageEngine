#pragma once

#include "GE_pch.h"

#include "../Entity.h"

#include "../Components/Light.h"

#include "Runtime/function/Render/Shared/LightUniform.h"

namespace GE
{
    class SceneLightManager
    {
    public:
        SceneLightManager(Scene& sc) : m_scene(sc) {};
        void Setup();
        void Destroy();

        inline bool GetChangedState() { return m_changed; }
        inline void ResetChangedState() { m_changed = false; }

        LightUniform                    GetLightUniformData();
        std::vector<PointLightData>     GetPointLightData();
        std::vector<SpotLightData>      GetSpotLightData();
        std::vector<DirectionLightData> GetDirectionLightData();

    private:
        void AddEntity(Entity& entity);
        void RemoveEntity(Entity& entity);
        void UpdateEntity(Entity& entity);

        std::map<LightType, std::map<int, std::shared_ptr<Entity>>> m_lightLists;

        bool m_changed = true;

        Scene& m_scene;
    };
} // namespace GE
