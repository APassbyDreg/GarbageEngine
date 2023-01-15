#pragma once

#include "GE_pch.h"

#include "Runtime/core/Concepts.h"

#include "Mesh.h"

namespace GE
{
    class GE_API MeshFactory : public Singleton<MeshFactory>
    {
        using FactoryFuncNoData   = std::function<std::shared_ptr<Mesh>(int id, fs::path path)>;
        using FactoryFuncWithData = std::function<std::shared_ptr<Mesh>(int id, fs::path path, const json&)>;

    public:
        inline static const std::vector<std::string>& GetSupportedMesh() { return GetInstance().m_meshNames; }

        inline static std::shared_ptr<Mesh> CreateMesh(std::string name, int id, fs::path path)
        {
            GetInstance().EnsureInit();
            return GetInstance().m_factoriesNoData[name](id, path);
        }
        inline static std::shared_ptr<Mesh> CreateMesh(std::string name, int id, fs::path path, const json& data)
        {
            GetInstance().EnsureInit();
            return GetInstance().m_factoriesWithData[name](id, path, data);
        }

    private:
        template<std::derived_from<Mesh> T>
        void AddMesh()
        {
            auto name = T::GetNameStatic();
            m_meshNames.push_back(name);
            m_factoriesNoData[name]   = [](int id, fs::path path) { return std::make_shared<T>(id, path); };
            m_factoriesWithData[name] = [](int id, fs::path path, const json& data) {
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

        inline static std::shared_ptr<Mesh> CreateMesh(std::string name, fs::path path)
        {
            uint id   = GetInstance().m_meshs.size();
            auto Mesh = MeshFactory::CreateMesh(name, id, path);
            GetInstance().m_meshs.push_back(Mesh);
            return Mesh;
        }
        inline static std::shared_ptr<Mesh> CreateDeferredMesh(std::string name, fs::path path, const json& data)
        {
            uint id   = GetInstance().m_meshs.size();
            auto mesh = MeshFactory::CreateMesh(name, id, path, data);
            GetInstance().m_meshs.push_back(mesh);
            return mesh;
        }

    private:
        std::vector<std::shared_ptr<Mesh>> m_meshs;
    };
} // namespace GE