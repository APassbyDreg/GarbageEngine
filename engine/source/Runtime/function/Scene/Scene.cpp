#include "Scene.h"

#include "Runtime/function/Scene/Components/Tag.h"

#include "Runtime/core/Math/Random.h"

namespace GE
{
#define SETUP_SCENE_MANAGERS() m_MeshManager(*this), m_CameraManager(*this)

    Scene::Scene() : SETUP_SCENE_MANAGERS()
    {
        RandomEngine rand;
        m_name = std::format("GE_scene_{}", rand.RandInt<uint>(0, (1 << 16) - 1));
        Setup();
    }

    Scene::Scene(const json& data) : SETUP_SCENE_MANAGERS() { Deserialize(data); }

    Scene::Scene(const fs::path path) : SETUP_SCENE_MANAGERS() { Load(path); }

    Scene::~Scene() { Destroy(); }

    void Scene::Setup()
    {
        m_MeshManager.Setup();
        m_CameraManager.Setup();
        SceneSettingsFactory::InitializeSettingsMap(m_sceneSettings);
    }

    void Scene::Destroy()
    {
        m_CameraManager.Destroy();
        m_MeshManager.Destroy();
    }

    std::shared_ptr<Entity> Scene::CreateEntity(uint eid, std::string tagname, int layer, int tag)
    {
        GE_CORE_ASSERT(eid >= 0, "Entity ID must be greater than or equal to 0");
        if (m_entities.find(eid) != m_entities.end())
        {
            while (m_entities.find(m_availableEntityID) != m_entities.end())
            {
                eid++;
            }
        }
        auto e = std::make_shared<Entity>(*this, eid);
        e->AddComponent<TagComponent>(tagname, layer, tag);
        m_entities[eid] = e;
        m_entityToID[e->m_registryID] = eid;
        return e;
    }

    std::shared_ptr<Entity> Scene::CreateEntity(std::string tagname, int layer, int tag)
    {
        while (m_entities.find(m_availableEntityID) != m_entities.end())
        {
            m_availableEntityID++;
        }
        auto e = std::make_shared<Entity>(*this, m_availableEntityID);
        e->AddComponent<TagComponent>(tagname, layer, tag);
        m_entities[m_availableEntityID] = e;
        return e;
    }

    void Scene::RemoveEntity(uint eid)
    {
        auto found = m_entities.find(eid);
        if (found != m_entities.end())
        {
            m_entityToID.erase(found->second->m_registryID);
            m_entities.erase(found);
        }
    }

    std::shared_ptr<Entity> Scene::GetEntityByID(int eid)
    {
        auto found = m_entities.find(eid);
        if (eid < 0 || found == m_entities.end())
        {
            return nullptr;
        }
        else
        {
            return found->second;
        }
    };

    std::shared_ptr<Entity> Scene::GetEntityByName(std::string name)
    {
        for (auto&& [eid, e] : m_entities)
        {
            auto&& tagname = e->GetComponent<TagComponent>().GetTagName();
            if (tagname == name)
            {
                return e;
            }
        }
        return nullptr;
    }

    json Scene::Serialize() const
    {
        json entities = json::array();
        for (auto&& [eid, e] : m_entities)
        {
            entities.push_back(e->Serialize());
        }

        json settings;
        for (auto&& [name, val] : m_sceneSettings)
        {
            settings[name] = val->Serialize();
        }

        json root;
        root["name"]     = m_name;
        root["entities"] = entities;
        root["settings"] = settings;

        return root;
    }

    void Scene::Deserialize(const json& data)
    {
        m_name = data["name"].get<std::string>();
        Setup();

        for (auto& edata : data["entities"])
        {
            int eid         = edata["id"].get<int>();
            m_entities[eid] = std::make_shared<Entity>(*this, eid);
            m_entities[eid]->Deserialize(edata);
        }
        SetupEntityInheritance();

        for (auto&& [sname, sval] : data["settings"].items())
        {
            auto setting           = SceneSettingsFactory::CreateSceneSetting(sname, sval);
            m_sceneSettings[sname] = setting;
        }
    }

    void Scene::Load(fs::path path)
    {
        if (m_resource != nullptr) // save previous work
        {
            m_resource->SaveData(Serialize());
        }
        m_resource = ResourceManager::GetResource<JsonResource>(path, JsonIdentifier::SCENE_DESCRIPTION);
        Deserialize(m_resource->GetData());
    }

    void Scene::Save(fs::path path, const bool save_as)
    {
        json data  = Serialize();
        bool saved = false;

        if (m_resource != nullptr) // save previous work
        {
            m_resource->SaveData(data);
            saved = true;
        }

        if (path != "" && (save_as || m_resource == nullptr)) // save a new file
        {
            m_resource = ResourceManager::GetResource<JsonResource>(path, JsonIdentifier::SCENE_DESCRIPTION);
            m_resource->SaveData(data);
            saved = true;
        }

        if (!saved)
        {
            GE_CORE_WARN("[Scene::Save] Failed to save because both previous and given path is empty!");
        }
    }

    void Scene::SetupEntityInheritance()
    {
        for (auto&& [eid, entity] : m_entities)
        {
            std::shared_ptr<Entity> parent = entity->GetParent();
            if (parent != nullptr)
            {
                parent->m_children.push_back(entity);
            }
        }
    }
} // namespace GE