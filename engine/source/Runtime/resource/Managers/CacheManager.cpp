#include "CacheManager.h"

#include "Runtime/core/Hash.h"
#include "Runtime/core/Packing.h"
#include "miniz.h"

namespace GE
{
    void __save_cache(std::string path, const char* content, uint64 size, std::string& sha)
    {
        uint64 cmp_size;
        uchar* cmp_data;

        Packing::CompressData((uchar*)content, size, &cmp_data, cmp_size);

        std::ofstream file(path, std::ios::binary);
        file.write((char*)&size, sizeof(uint64));
        file.write((char*)&cmp_size, sizeof(uint64));
        file.write((char*)cmp_data, cmp_size);
        file.close();

        sha = sha256((const char*)cmp_data, cmp_size);

        delete[] cmp_data;
    }

    void __load_cache(std::string path, char** content, uint64& size, std::string& sha)
    {
        // load basic data
        uint64        cmp_size;
        char*         cmp_data;
        std::ifstream file(path, std::ios::binary);
        file.read((char*)&size, sizeof(uint64));
        file.read((char*)&cmp_size, sizeof(uint64));

        // alloc memory
        *content = new char[size];
        cmp_data = new char[cmp_size];
        file.read((char*)cmp_data, cmp_size);

        // compute sha on compressed data
        sha = sha256((const char*)cmp_data, cmp_size);

        // decompress
        if (size > cmp_size)
        {
            Packing::DecompressData((uchar*)cmp_data, cmp_size, (uchar**)content, size);
        }
        else
        {
            memcpy(*content, cmp_data, cmp_size);
        }

        file.close();

        delete[] cmp_data;
    }

    CacheManager::CacheManager()
    {
        if (!fs::exists(cache_dir))
        {
            fs::create_directories(cache_dir);
        }
        if (fs::exists(cache_desc))
        {
            std::ifstream file(cache_desc);
            file >> cache_map;
        }
    }

    CacheManager::~CacheManager() { UpdateCacheDesc(); }

    bool CacheManager::Load(CacheDescriptor desc, char** content, uint64& size)
    {
        // find key
        if (cache_map.find(desc.name) == cache_map.end())
        {
            return false;
        }
        // check specifier
        if (cache_map[desc.name]["specifier"] != desc.specifier || cache_map[desc.name]["type"] != desc.type)
        {
            cache_map.erase(desc.name);
            UpdateCacheDesc();
            return false;
        }
        // try load
        fs::path cache_path = cache_dir / (sha256(desc.name) + ".cache");
        if (fs::exists(cache_path))
        {
            std::string cmp_content_hash;
            __load_cache(cache_path.string(), content, size, cmp_content_hash);
            if (cmp_content_hash != cache_map[desc.name]["content_hash"])
            {
                cache_map.erase(desc.name);
                UpdateCacheDesc();
                return false;
            }
            return true;
        }

        return false;
    }

    bool CacheManager::Save(CacheDescriptor desc, const char* content, uint64 size)
    {
        json block = {
            {"specifier", desc.specifier},
            {"type", desc.type},
            {"name_hash", sha256(desc.name)},
        };

        fs::path    cache_path = cache_dir / (sha256(desc.name) + ".cache");
        std::string cmp_content_hash;

        __save_cache(cache_path.string(), content, size, cmp_content_hash);

        block["content_hash"] = cmp_content_hash;
        cache_map[desc.name]  = block;

        UpdateCacheDesc();

        return true;
    }

    void CacheManager::UpdateCacheDesc()
    {
        std::ofstream file(cache_desc);
        file << cache_map;
    }

    uint CacheManager::Invalidate(InvalidateFn fn)
    {
        std::vector<std::string> rm_keys = {};
        for (auto& [name, block] : cache_map.items())
        {
            if (fn(name, block["type"], block["specifier"]))
            {
                fs::path cache_path = cache_dir / (sha256(name) + ".cache");
                fs::remove(cache_path);
                rm_keys.push_back(name);
            }
        }
        for (auto& key : rm_keys)
        {
            cache_map.erase(key);
        }
        UpdateCacheDesc();
        return rm_keys.size();
    }
} // namespace GE