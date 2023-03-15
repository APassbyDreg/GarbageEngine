#pragma once

#include "GE_pch.h"

#include "Runtime/core/Concepts.h"

#include "Material.h"
#include <memory>

namespace GE
{
    class GE_API MaterialFactory : public Singleton<MaterialFactory>
    {
        template<std::derived_from<Material> T>
        using FactoryFuncNoData = std::function<std::shared_ptr<T>(int id, fs::path path)>;
        template<std::derived_from<Material> T>
        using FactoryFuncWithData = std::function<std::shared_ptr<T>(int id, fs::path path, const json&)>;

    public:
        template<IsAnyOf<ForwardMaterial, DeferredMaterial> T>
        inline static const std::vector<std::string>& GetSupportedMaterial()
        {
            GetInstance().EnsureInit();
            if constexpr (std::is_same_v<T, ForwardMaterial>)
            {
                return GetInstance().m_forwardMaterialTypes;
            }
            if constexpr (std::is_same_v<T, DeferredMaterial>)
            {
                return GetInstance().m_deferredMaterialTypes;
            }
        }

        inline static std::shared_ptr<ForwardMaterial> CreateForwardMaterial(std::string type, int id, fs::path path)
        {
            GetInstance().EnsureInit();
            return GetInstance().m_forwardFactoriesNoData[type](id, path);
        }
        inline static std::shared_ptr<ForwardMaterial>
        CreateForwardMaterial(std::string type, int id, fs::path path, const json& data)
        {
            GetInstance().EnsureInit();
            return GetInstance().m_forwardFactoriesWithData[type](id, path, data);
        }

        inline static std::shared_ptr<DeferredMaterial> CreateDeferredMaterial(std::string type, int id, fs::path path)
        {
            GetInstance().EnsureInit();
            return GetInstance().m_deferredFactoriesNoData[type](id, path);
        }
        inline static std::shared_ptr<DeferredMaterial>
        CreateDeferredMaterial(std::string type, int id, fs::path path, const json& data)
        {
            GetInstance().EnsureInit();
            return GetInstance().m_deferredFactoriesWithData[type](id, path, data);
        }

    private:
        template<std::derived_from<ForwardMaterial> T>
        void AddForwardMaterial()
        {
            auto type = T::GetTypeStatic();
            m_forwardMaterialTypes.push_back(type);
            m_forwardFactoriesNoData[type]   = [](int id, fs::path path) { return std::make_shared<T>(id, path); };
            m_forwardFactoriesWithData[type] = [](int id, fs::path path, const json& data) {
                return std::make_shared<T>(id, path, data);
            };
        }

        template<std::derived_from<DeferredMaterial> T>
        void AddDeferredMaterial()
        {
            auto type = T::GetTypeStatic();
            m_deferredMaterialTypes.push_back(type);
            m_deferredFactoriesNoData[type]   = [](int id, fs::path path) { return std::make_shared<T>(id, path); };
            m_deferredFactoriesWithData[type] = [](int id, fs::path path, const json& data) {
                return std::make_shared<T>(id, path, data);
            };
        }
        void EnsureInit();

    private:
        bool m_initialized = false;

        std::vector<std::string>                                     m_forwardMaterialTypes;
        std::vector<std::string>                                     m_deferredMaterialTypes;
        std::map<std::string, FactoryFuncNoData<ForwardMaterial>>    m_forwardFactoriesNoData;
        std::map<std::string, FactoryFuncNoData<DeferredMaterial>>   m_deferredFactoriesNoData;
        std::map<std::string, FactoryFuncWithData<ForwardMaterial>>  m_forwardFactoriesWithData;
        std::map<std::string, FactoryFuncWithData<DeferredMaterial>> m_deferredFactoriesWithData;
    };

    class GE_API MaterialManager : public Singleton<MaterialManager>
    {
    public:
        ~MaterialManager()
        {
            for (auto&& mat : m_forwardMaterials)
            {
                mat->Save();
            }
            for (auto&& mat : m_deferredMaterials)
            {
                mat->Save();
            }
        }

        template<IsAnyOf<ForwardMaterial, DeferredMaterial> T>
        inline static const std::vector<std::string>& GetSupportedMaterial()
        {
            return MaterialFactory::GetSupportedMaterial<T>();
        }

        inline static std::vector<std::shared_ptr<DeferredMaterial>> GetDeferredMaterialList()
        {
            return GetInstance().m_deferredMaterials;
        }
        inline static std::vector<std::shared_ptr<ForwardMaterial>> GetForwardMaterialList()
        {
            return GetInstance().m_forwardMaterials;
        }

        inline static std::shared_ptr<DeferredMaterial> GetDeferredMaterialByID(uint id)
        {
            return GetInstance().m_deferredMaterials[id];
        }
        inline static std::shared_ptr<ForwardMaterial> GetForwardMaterialByID(uint id)
        {
            return GetInstance().m_forwardMaterials[id];
        }

        inline static std::shared_ptr<ForwardMaterial> CreateForwardMaterial(std::string type, std::string path)
        {
            uint id       = GetInstance().m_forwardMaterials.size();
            auto material = MaterialFactory::CreateForwardMaterial(type, id, path);
            GetInstance().m_forwardMaterials.push_back(material);
            GetInstance().m_pathToMaterial[path] = material;
            return material;
        }
        inline static std::shared_ptr<DeferredMaterial> CreateDeferredMaterial(std::string type, std::string path)
        {
            uint id       = GetInstance().m_deferredMaterials.size();
            auto material = MaterialFactory::CreateDeferredMaterial(type, id, path);
            GetInstance().m_deferredMaterials.push_back(material);
            GetInstance().m_pathToMaterial[path] = material;
            return material;
        }
        inline static std::shared_ptr<ForwardMaterial>
        CreateForwardMaterial(std::string type, std::string path, const json& data)
        {
            uint id       = GetInstance().m_forwardMaterials.size();
            auto material = MaterialFactory::CreateForwardMaterial(type, id, path, data);
            GetInstance().m_forwardMaterials.push_back(material);
            GetInstance().m_pathToMaterial[path] = material;
            return material;
        }
        inline static std::shared_ptr<DeferredMaterial>
        CreateDeferredMaterial(std::string type, std::string path, const json& data)
        {
            uint id       = GetInstance().m_deferredMaterials.size();
            auto material = MaterialFactory::CreateDeferredMaterial(type, id, path, data);
            GetInstance().m_deferredMaterials.push_back(material);
            GetInstance().m_pathToMaterial[path] = material;
            return material;
        }

        inline static std::shared_ptr<Material> LoadMaterial(std::string path)
        {
            auto&& instance = GetInstance();
            if (instance.m_pathToMaterial.find(path) != instance.m_pathToMaterial.end())
            {
                return instance.m_pathToMaterial[path];
            }

            auto&& resource = ResourceManager::GetResource<JsonResource>(path, JsonIdentifier::MATERIAL);
            if (resource->IsValid())
            {
                auto&& data = resource->GetData();
                auto&& type = data["type"].get<std::string>();
                auto&& mode = data["mode"].get<std::string>();
                if (mode == "forward")
                {
                    return CreateForwardMaterial(type, path);
                }
                if (mode == "deferred")
                {
                    return CreateDeferredMaterial(type, path);
                }
            }
            GE_CORE_WARN("[MaterialManager::LoadMaterial] invalid input path [{}]", path);
            return nullptr;
        }

        inline static std::shared_ptr<Material> CreateMaterial(std::string type, std::string mode, std::string path)
        {
            auto&& instance = GetInstance();
            if (instance.m_pathToMaterial.find(path) != instance.m_pathToMaterial.end())
            {
                return instance.m_pathToMaterial[path];
            }
            if (mode == "forward")
            {
                return CreateForwardMaterial(type, path);
            }
            if (mode == "deferred")
            {
                return CreateDeferredMaterial(type, path);
            }
            GE_CORE_WARN("[MaterialManager::CreateMaterial] invalid input mode [{}]", mode);
            return nullptr;
        }

        inline static void SaveAll()
        {
            for (auto&& mat : GetInstance().m_forwardMaterials)
            {
                mat->Save();
            }
            for (auto&& mat : GetInstance().m_deferredMaterials)
            {
                mat->Save();
            }
        }

    private:
        std::vector<std::shared_ptr<DeferredMaterial>> m_deferredMaterials;
        std::vector<std::shared_ptr<ForwardMaterial>>  m_forwardMaterials;
        std::map<std::string, std::shared_ptr<Material>> m_pathToMaterial;
    };
} // namespace GE