#include "SceneSettingsFactory.h"

/* ------------------------- register scene settings ------------------------ */
#include "TestSceneSetting.h"

namespace GE
{
#define REGISTER_SCENE_SETTING(name) \
    m_sceneSettingFactories[#name]    = [](const json& data) { return std::make_shared<name>(data); }; \
    m_sceneSettingInitializers[#name] = []() { return std::make_shared<name>(); };

    void SceneSettingsFactory::EnsureInit()
    {
        if (!m_initialized)
        {
            REGISTER_SCENE_SETTING(TestSceneSetting);

            m_initialized = true;
        }
    }

#undef REGISTER_SCENE_SETTING
} // namespace GE
