#include "Scene.h"

#include "Runtime/function/Scene/Components/Tag.h"

namespace GE
{
    Scene::Scene() : m_MeshManager(*this)
    {
        Setup();
        m_name = std::format("GE_scene_{}", std::rand() % (1 << 16));
    }

    Scene::Scene(const json& data) : m_MeshManager(*this)
    {
        Setup();
        Deserialize(data);
    }

    Scene::Scene(const fs::path path) : m_MeshManager(*this)
    {
        Setup();
        Load(path);
    }

    Scene::~Scene() {}

    void Scene::Setup()
    {
        m_MeshManager.Setup();
        SceneSettingsFactory::InitializeSettingsMap(m_sceneSettings);
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
            TagComponent& tag = e->GetComponent<TagComponent>();
            if (tag.GetCoreValues().name == name)
            {
                return e;
            }
        }
        return nullptr;
    }

    void Scene::InspectSettings()
    {
        for (auto&& [name, setting] : m_sceneSettings)
        {
            if (ImGui::CollapsingHeader(name.c_str()))
            {
                setting->Inspect();
            }
        }
    }

    void Scene::InspectStructure()
    {
        strcpy(m_nameBuffer, m_name.c_str());
        ImGui::InputText("Scene Name", m_nameBuffer, 256);
        m_name = m_nameBuffer;

        ImGui::Separator();
        ImGui::Text("Entities");

        int idx = 0;
        for (auto&& [eid, e] : m_entities)
        {
            TagComponent& tag  = e->GetComponent<TagComponent>();
            std::string   name = tag.GetCoreValues().name;
            if (name.empty())
            {
                name = "unnamed entity";
            }
            if (ImGui::Selectable(name.c_str(), m_focusEntityID == idx))
            {
                m_focusEntityID = eid;
            }
            idx++;
        }
    }

    void Scene::InspectFocusedEntity()
    {
        std::shared_ptr<Entity> focused_entity = GetEntityByID(m_focusEntityID);
        if (focused_entity != nullptr)
        {
            std::vector<std::string> add_list    = {};
            std::vector<std::string> remove_list = {};

            // add new component
            if (ImGui::Button("Add Component.."))
                ImGui::OpenPopup("ge_add_component_popup");
            if (ImGui::BeginPopup("ge_add_component_popup"))
            {
                for (auto&& name : ComponentFactory::GetInstance().GetSupportedComponents())
                {
                    if (ImGui::Selectable(name.c_str()))
                        ComponentFactory::GetInstance().AttachComponent(name, *focused_entity);
                }
                ImGui::EndPopup();
            }

            // parent entity selector
            if (ImGui::Button("Select Parent.."))
                ImGui::OpenPopup("ge_parent_entity_popup");
            if (ImGui::BeginPopup("ge_parent_entity_popup"))
            {
                for (auto [eid, entity] : m_entities)
                {
                    std::string display_name =
                        std::to_string(eid) + ": " + entity->GetComponent<TagComponent>().GetCoreValues().name;
                    if (ImGui::Selectable(display_name.c_str()))
                    {
                        focused_entity->SetParent(entity);
                    }
                }
                ImGui::EndPopup();
            }

            // display parent
            ImGui::SameLine();
            std::shared_ptr<Entity> parent = focused_entity->GetParent();
            std::string             parent_text =
                "Parent: " + ((parent == nullptr) ? "None" : parent->GetComponent<TagComponent>().GetCoreValues().name);
            ImGui::Text(parent_text.c_str());

            // per component
            uint idx = 0;
            focused_entity->IterateComponent([&](ComponentBase& comp, Entity& e) {
                if (ImGui::CollapsingHeader(comp.GetName().c_str()))
                {
                    if (comp.GetName() != TagComponent::GetNameStatic())
                    {
                        ImGui::PushID(idx);
                        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
                        if (ImGui::Button("remove", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
                        {
                            remove_list.push_back(comp.GetName());
                        }
                        ImGui::PopStyleColor(3);
                        ImGui::PopID();
                    }
                    comp.Inspect();
                }
                ImGui::Separator();
                idx++;
            });

            for (auto&& name : remove_list)
            {
                ComponentFactory::GetInstance().DetachComponent(name, *m_entities[m_focusEntityID]);
            }
        }
        else
        {
            m_focusEntityID = -1;
        }
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