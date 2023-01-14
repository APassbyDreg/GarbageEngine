#pragma once

#include "GE_pch.h"

#include "Runtime/core/Hash.h"
#include "Runtime/core/Math/Math.h"

#include "Runtime/core/Log/LogSystem.h"

namespace GE
{
    enum class ResourceType
    {
        BYTE,
        JSON,
        MESH,
        TEXT,
        IMAGE
    };

    class ResourceBase
    {
    public:
        ResourceBase(ResourceType type,
                     fs::path     file,
                     bool         init         = false,
                     bool         use_cache    = false,
                     bool         delayed_load = false) :
            m_type(type),
            m_filePath(file), m_cacheEnabled(use_cache)
        {
            if (!delayed_load)
                Load();
            if (init && !m_valid)
                Initialize();
        }
        ~ResourceBase()
        {
            if (IsValid())
            {
                Save();
            }
        }

        inline fs::path    GetFilePath() { return m_filePath; }
        inline std::string GetExtension() { return m_filePath.extension().string(); }
        inline bool        IsValid() const { return m_valid; }

        virtual void Load() {}
        virtual void Save() {}
        virtual void Initialize() { m_valid = true; }

    protected:
        fs::path     m_filePath     = "";
        bool         m_valid        = false;
        bool         m_cacheEnabled = false;
        ResourceType m_type;
    };

    template<typename T>
    class Resource : public ResourceBase
    {
    public:
        Resource(ResourceType type,
                 fs::path     file,
                 bool         init         = false,
                 bool         use_cache    = false,
                 bool         delayed_load = false) :
            ResourceBase(type, file, use_cache, delayed_load)
        {}

        inline T& GetData()
        {
            if (!m_valid)
                Load();
            if (!m_valid)
                GE_CORE_WARN("[Resource::GetData] Resource {0} not loaded", m_filePath.string());
            return m_data;
        }

        virtual void SaveData(const T& data) = 0;
        virtual void Invalid()               = 0;

    protected:
        T m_data;
    };
} // namespace GE