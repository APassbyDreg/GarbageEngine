#pragma once

#include "GE_pch.h"

#include "Runtime/core/Concepts.h"

#include "Runtime/core/Utils/ContainerUtils.h"

#include "Mesh.h"

namespace GE
{
    class GE_API MeshFactory : public Singleton<MeshFactory>
    {
        using FactoryFuncNoData   = std::function<std::shared_ptr<Mesh>(int id, fs::path path)>;
        using FactoryFuncWithData = std::function<std::shared_ptr<Mesh>(int id, fs::path path, const json&)>;

    public:
        inline static const std::vector<std::string>& GetSupportedMesh()
        {
            GetInstance().EnsureInit();
            return GetInstance().m_meshNames;
        }

        inline static std::shared_ptr<Mesh> CreateMesh(std::string type, int id, fs::path path)
        {
            GetInstance().EnsureInit();
            return GetInstance().m_factoriesNoData[type](id, path);
        }
        inline static std::shared_ptr<Mesh> CreateMesh(std::string type, int id, fs::path path, const json& data)
        {
            GetInstance().EnsureInit();
            return GetInstance().m_factoriesWithData[type](id, path, data);
        }

    private:
        template<std::derived_from<Mesh> T>
        void AddMesh()
        {
            auto type = T::GetTypeStatic();
            m_meshNames.push_back(type);
            m_factoriesNoData[type]   = [](int id, fs::path path) { return std::make_shared<T>(id, path); };
            m_factoriesWithData[type] = [](int id, fs::path path, const json& data) {
                return std::make_shared<T>(id, path, data);
            };
        }

        void EnsureInit();

    private:
        bool m_initialized = false;

        std::vector<std::string>                   m_meshNames;
        std::map<std::string, FactoryFuncNoData>   m_factoriesNoData;
        std::map<std::string, FactoryFuncWithData> m_factoriesWithData;
    };

    class GE_API MeshManager : public Singleton<MeshManager>
    {
    public:
        ~MeshManager()
        {
            for (auto&& mesh : m_meshs)
            {
                mesh->Save();
            }
        }

        inline static const std::vector<std::string>& GetSupportedMesh() { return MeshFactory::GetSupportedMesh(); }

        inline static std::vector<std::shared_ptr<Mesh>> GetMeshList() { return GetInstance().m_meshs; }

        inline static std::shared_ptr<Mesh> GetMeshByID(uint id) { return GetInstance().m_meshs[id]; }

        inline static std::shared_ptr<Mesh> GetMeshByTypeAndFilePath(std::string type, std::string path)
        {
            auto&& instance = GetInstance();
            if (instance.m_pathToMesh.find(path) != instance.m_pathToMesh.end())
            {
                auto&& mesh = instance.m_pathToMesh[path];
                if (mesh->GetType() == type)
                    return mesh;
                else
                    return nullptr;
            }
            else
            {
                return CreateMesh(type, path);
            }
        }

        inline static std::shared_ptr<Mesh> LoadMesh(std::string path)
        {
            auto&& instance = GetInstance();
            if (StdUtils::Exists(instance.m_pathToMesh, path))
            {
                return instance.m_pathToMesh[path];
            }

            uint   id       = instance.m_meshs.size();
            auto&& resource = ResourceManager::GetResource<JsonResource>(path, JsonIdentifier::MESH);
            if (!resource->IsValid())
            {
                GE_CORE_WARN("[MeshManager::LoadMesh] trying to read from an none-exist path {}", path);
                return nullptr;
            }
            auto&& type = resource->GetData()["type"].get<std::string>();
            auto&& mesh = MeshFactory::CreateMesh(type, id, path);
            instance.m_meshs.push_back(mesh);
            instance.m_pathToMesh[path] = mesh;
            return mesh;
        }

        inline static std::shared_ptr<Mesh> CreateMesh(std::string type, std::string path)
        {
            auto&& instance = GetInstance();

            uint   id   = instance.m_meshs.size();
            auto&& mesh = MeshFactory::CreateMesh(type, id, path);
            instance.m_meshs.push_back(mesh);
            instance.m_pathToMesh[path] = mesh;
            return mesh;
        }

        inline static void SaveAll()
        {
            for (auto&& mesh : GetInstance().m_meshs)
            {
                mesh->Save();
            }
        }

    private:
        std::vector<std::shared_ptr<Mesh>>           m_meshs;
        std::map<std::string, std::shared_ptr<Mesh>> m_pathToMesh;
    };
} // namespace GE