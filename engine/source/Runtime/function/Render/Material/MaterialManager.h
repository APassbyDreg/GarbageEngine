#pragma once

#include "GE_pch.h"

#include "Runtime/core/Concepts.h"

#include "Material.h"

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
                return GetInstance().m_forwardMaterialNames;
            }
            if constexpr (std::is_same_v<T, DeferredMaterial>)
            {
                return GetInstance().m_deferredMaterialNames;
            }
        }

        inline static std::shared_ptr<ForwardMaterial> CreateForwardMaterial(std::string name, int id, fs::path path)
        {
            GetInstance().EnsureInit();
            return GetInstance().m_forwardFactoriesNoData[name](id, path);
        }
        inline static std::shared_ptr<ForwardMaterial>
        CreateForwardMaterial(std::string name, int id, fs::path path, const json& data)
        {
            GetInstance().EnsureInit();
            return GetInstance().m_forwardFactoriesWithData[name](id, path, data);
        }

        inline static std::shared_ptr<DeferredMaterial> CreateDeferredMaterial(std::string name, int id, fs::path path)
        {
            GetInstance().EnsureInit();
            return GetInstance().m_deferredFactoriesNoData[name](id, path);
        }
        inline static std::shared_ptr<DeferredMaterial>
        CreateDeferredMaterial(std::string name, int id, fs::path path, const json& data)
        {
            GetInstance().EnsureInit();
            return GetInstance().m_deferredFactoriesWithData[name](id, path, data);
        }

    private:
        template<std::derived_from<ForwardMaterial> T>
        void AddForwardMaterial()
        {
            auto name = T::GetNameStatic();
            m_forwardMaterialNames.push_back(name);
            m_forwardFactoriesNoData[name]   = [](int id, fs::path path) { return std::make_shared<T>(id, path); };
            m_forwardFactoriesWithData[name] = [](int id, fs::path path, const json& data) {
                return std::make_shared<T>(id, path, data);
            };
        }

        template<std::derived_from<DeferredMaterial> T>
        void AddDeferredMaterial()
        {
            auto name = T::GetNameStatic();
            m_deferredMaterialNames.push_back(name);
            m_deferredFactoriesNoData[name]   = [](int id, fs::path path) { return std::make_shared<T>(id, path); };
            m_deferredFactoriesWithData[name] = [](int id, fs::path path, const json& data) {
                return std::make_shared<T>(id, path, data);
            };
        }
        void EnsureInit();

    private:
        bool m_initialized = false;

        std::vector<std::string>                                     m_forwardMaterialNames;
        std::vector<std::string>                                     m_deferredMaterialNames;
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

        inline static std::shared_ptr<ForwardMaterial> CreateForwardMaterial(std::string name, fs::path path)
        {
            uint id       = GetInstance().m_forwardMaterials.size();
            auto material = MaterialFactory::CreateForwardMaterial(name, id, path);
            GetInstance().m_forwardMaterials.push_back(material);
            return material;
        }
        inline static std::shared_ptr<DeferredMaterial> CreateDeferredMaterial(std::string name, fs::path path)
        {
            uint id       = GetInstance().m_deferredMaterials.size();
            auto material = MaterialFactory::CreateDeferredMaterial(name, id, path);
            GetInstance().m_deferredMaterials.push_back(material);
            return material;
        }
        inline static std::shared_ptr<ForwardMaterial>
        CreateForwardMaterial(std::string name, fs::path path, const json& data)
        {
            uint id       = GetInstance().m_forwardMaterials.size();
            auto material = MaterialFactory::CreateForwardMaterial(name, id, path, data);
            GetInstance().m_forwardMaterials.push_back(material);
            return material;
        }
        inline static std::shared_ptr<DeferredMaterial>
        CreateDeferredMaterial(std::string name, fs::path path, const json& data)
        {
            uint id       = GetInstance().m_deferredMaterials.size();
            auto material = MaterialFactory::CreateDeferredMaterial(name, id, path, data);
            GetInstance().m_deferredMaterials.push_back(material);
            return material;
        }

    private:
        std::vector<std::shared_ptr<DeferredMaterial>> m_deferredMaterials;
        std::vector<std::shared_ptr<ForwardMaterial>>  m_forwardMaterials;
    };
} // namespace GE