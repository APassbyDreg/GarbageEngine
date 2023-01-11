#pragma once

#include "GE_pch.h"

#include "Runtime/core/Base/Singleton.h"

#include "../ResourceBase.h"

namespace GE
{
    class GE_API ResourceManager : public Singleton<ResourceManager, -999>
    {
    public:
        template<std::derived_from<ResourceBase> T, typename... TArgs>
        static std::shared_ptr<T> GetResource(fs::path file, TArgs&&... args)
        {
            auto& instance = GetInstance();
            if (instance.m_resources.find(file) == instance.m_resources.end())
            {
                instance.m_resources[file] = std::make_shared<T>(file, std::forward<TArgs>(args)...);
            }
            return std::dynamic_pointer_cast<T>(instance.m_resources[file]);
        }

    private:
        std::map<fs::path, std::shared_ptr<ResourceBase>> m_resources = {};
    };
} // namespace GE