#pragma once

#include "GE_pch.h"

#include "core/Json.h"
#include "core/base/Singleton.h"
#include "core/math/math.h"

namespace GE
{

    struct CacheDescriptor
    {
        CacheDescriptor(std::string _name, std::string _specifier, std::string _type = "") :
            name(_name), specifier(_specifier), type(_type)
        {}

        std::string name;
        std::string type;      // additional info for cache
        std::string specifier; // user provided string to validate the cache
    };

    class CacheManager : public Singleton<CacheManager>
    {
    public:
        CacheManager();
        ~CacheManager();

        bool Load(CacheDescriptor desc, char** content, uint64& size);

        bool Save(CacheDescriptor desc, const char* content, uint64 size);

        using InvalidateFn = std::function<bool(std::string, std::string, std::string)>;
        uint Invalidate(InvalidateFn fn);

    private:
        void UpdateCacheDesc();

        /**
         * @brief a map of cache items
         * key: hash(cache name)
         * value: { specifier: hash(cache specifier), content hash: hash(cache content) }
         */
        json cache_map;

        // TODO: change to another directory
        const fs::path cache_dir  = fs::path(Config::asset_dir) / "cache";
        const fs::path cache_desc = cache_dir / "cache.json";
    };
} // namespace GE