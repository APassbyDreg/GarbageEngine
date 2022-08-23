#pragma once

#include "GE_pch.h"

#include "Runtime/core/Base/Singleton.h"

#include "../ResourceTypes/ByteResource.h"
#include "../ResourceTypes/ImageResource.h"
#include "../ResourceTypes/JsonResource.h"
#include "../ResourceTypes/MeshResource.h"
#include "../ResourceTypes/TextResource.h"


namespace GE
{
    class GE_API ResourceManager : public Singleton<ResourceManager>
    {
    public:
        template<std::derived_from<ResourceBase> T, typename... TArgs>
        std::shared_ptr<T> GetResource(fs::path file, TArgs&&... args)
        {
            if (m_resources.find(file) == m_resources.end())
            {
                m_resources[file] = std::make_shared<T>(file, std::forward<TArgs>(args)...);
            }
            return std::dynamic_pointer_cast<T>(m_resources[file]);
        }

    private:
        std::map<fs::path, std::shared_ptr<ResourceBase>> m_resources = {};
    };
} // namespace GE