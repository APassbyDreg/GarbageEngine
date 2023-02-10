#pragma once

#include "GE_pch.h"

#include "Entity.h"

#include "Runtime/function/Scene/Components/ComponentFactory.h"

#include "Runtime/resource/Managers/ResourceManager.h"
#include "Runtime/resource/ResourceTypes/JsonResource.h"

#include "Manager/CameraManager.h"
#include "Manager/MeshManager.h"

#include "Settings/SceneSettingsFactory.h"

namespace GE
{
#define DEFINE_SCENE_MANAGER(name) \
public: \
    Scene##name& Get##name() \
    { \
        return m_##name; \
    } \
\
private: \
    Scene##name m_##name;

    class GE_API Scene
    {
        friend class Entity;

        DEFINE_SCENE_MANAGER(MeshManager);
        DEFINE_SCENE_MANAGER(CameraManager);

    public:
        Scene();
        Scene(const json& data);
        Scene(const fs::path path);
        ~Scene();

        inline entt::registry& GetRegistry() { return m_registry; }

        /* -------------------------- create entity ------------------------- */
        std::shared_ptr<Entity>
        CreateEntity(uint eid, std::string tagname = "unnamed entity", int layer = 0, int tag = 0);
        std::shared_ptr<Entity> CreateEntity(std::string tagname = "unnamed entity", int layer = 0, int tag = 0);

        /* -------------------------- remove entity ------------------------- */
        void RemoveEntity(uint eid);

        /* --------------------------- get entity --------------------------- */
        std::shared_ptr<Entity> GetEntityByID(int eid);
        std::shared_ptr<Entity> GetEntityByName(std::string name);
        template<class... T>
        std::vector<std::shared_ptr<Entity>> GetEntitiesByComponent()
        {
            std::vector<std::shared_ptr<Entity>> entities;
            auto                                 view = m_registry.view<T...>();
            for (auto e : view)
            {
                entities.push_back(m_entities[m_entityToID[e]]);
            }
            return entities;
        }

        void InspectSettings();
        void InspectStructure();
        void InspectFocusedEntity();

        json Serialize() const;
        void Deserialize(const json& data);

        void Save(const fs::path path = "", const bool save_as = false);
        void Load(const fs::path path);

        inline std::string GetName() const { return m_name; }
        inline void        SetName(const std::string& name) { m_name = name; }

        inline json GetSetting(const std::string name) { return m_sceneSettings[name]->Serialize(); }
        inline void SetSetting(std::shared_ptr<SettingsBase> setting) { m_sceneSettings[setting->GetName()] = setting; }

    protected:
        entt::registry                          m_registry;
        std::map<uint, std::shared_ptr<Entity>> m_entities;
        std::map<entt::entity, uint>            m_entityToID;

        std::map<std::string, std::shared_ptr<SettingsBase>> m_sceneSettings;

        std::string m_name = "GE_scene";
        char        m_nameBuffer[256];

    private:
        int                           m_focusEntityID     = -1;
        uint                          m_availableEntityID = 0;
        std::shared_ptr<JsonResource> m_resource          = nullptr;

        void SetupEntityInheritance();
        void Setup();
    };

#undef DEFINE_SCENE_MANAGER
} // namespace GE