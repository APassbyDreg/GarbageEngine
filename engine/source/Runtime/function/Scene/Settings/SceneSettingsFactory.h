#pragma once

#include "Runtime/function/Settings/SettingsBase.h"

#include "Runtime/core/Log/LogSystem.h"

#include "Runtime/core/Base/Singleton.h"

namespace GE
{
    class SceneSettingsFactory : public Singleton<SceneSettingsFactory>
    {
        using SettingsFactoryFunc     = std::function<std::shared_ptr<SettingsBase>(const json&)>;
        using SettingsInitializerFunc = std::function<std::shared_ptr<SettingsBase>()>;

    public:
        static inline std::shared_ptr<SettingsBase> CreateSceneSetting(const std::string& name, const json& data)
        {
            GetInstance().EnsureInit();
            auto&& factories = GetInstance().m_sceneSettingFactories;
            if (factories.find(name) != factories.end())
            {
                return factories[name](data);
            }
            else
            {
                GE_CORE_ERROR("[SceneSettingsFactory::CreateSceneSetting] Scene setting {} not registered", name);
                return nullptr;
            }
        }

        static inline void InitializeSettingsMap(std::map<std::string, std::shared_ptr<SettingsBase>>& settings_map)
        {
            GetInstance().EnsureInit();
            for (auto&& [name, initializer] : GetInstance().m_sceneSettingInitializers)
            {
                settings_map[name] = initializer();
            }
        }

    private:
        void EnsureInit();
        bool m_initialized = false;

        std::map<std::string, SettingsFactoryFunc>     m_sceneSettingFactories    = {};
        std::map<std::string, SettingsInitializerFunc> m_sceneSettingInitializers = {};
    };
} // namespace GE