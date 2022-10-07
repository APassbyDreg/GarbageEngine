#include "resource.h"

#include "Runtime/function/Log/LogSystem.h"

#include "Runtime/resource/Managers/ResourceManager.h"
#include "Runtime/resource/ResourceTypes/MeshResource.h"

namespace GE
{
    void CacheManagerTestCase::run()
    {
        CacheManager::GetInstance().Save({"test1", "test1"}, "test1", 6);
        CacheManager::GetInstance().Save({"test2", "test2"}, "test2", 6);
        CacheManager::GetInstance().Save({"test3", "test3"}, "test3", 6);

        char*    res;
        uint64_t size;
        GE_CORE_ASSERT(CacheManager::GetInstance().Load({"test1", "test1"}, &res, size), "Failed to load test1");
        GE_CORE_ASSERT(size == 6 && strcmp(res, "test1") == 0,
                       "content error, expected test1, got: {}, size: {}",
                       std::string(res),
                       size);
        GE_CORE_ASSERT(!CacheManager::GetInstance().Load({"test?", "test?"}, &res, size), "Wrong content loaded");

        CacheManager::GetInstance().Invalidate([](std::string name, std::string type, std::string specifier) {
            return (name == "test1" && specifier == "test1") || (name == "test2" && specifier == "test2") ||
                   (name == "test3" && specifier == "test3");
        });
        GE_CORE_ASSERT(!CacheManager::GetInstance().Load({"test2", "test2"}, &res, size), "Failed to invalidate");

        fs::path obj_path      = fs::path(Config::asset_dir) / "mesh" / "basic" / "cube.obj";
        fs::path resource_path = fs::path(Config::asset_dir) / "test.mesh.ge";
        auto     resource      = ResourceManager::GetInstance().GetResource<MeshResource>(resource_path);
        resource->FromObj(obj_path);
        Mesh& mesh = resource->GetData();
        // resource->Save();

        // Mesh mesh;
        // for (int i = 0; i < 4; i++)
        // {
        //     mesh.vertices.push_back({});
        //     mesh.vertices[i].position = {i, i, i};
        // }
        // mesh.indices           = {0, 1, 2, 0, 2, 3};
        // fs::path path          = fs::path(Config::asset_dir) / "test.mesh.ge";
        // auto     mesh_resource = ResourceManager::GetInstance().GetResource<MeshResource>(path);
        // mesh_resource->SaveData(mesh);
        // mesh_resource->Invalid();
        // Mesh& loaded_mesh = mesh_resource->GetData();
        // for (int i = 0; i < 4; i++)
        // {
        //     auto v  = mesh.vertices[i];
        //     auto v_ = loaded_mesh.vertices[i];
        //     GE_CORE_ASSERT(
        //         v.position == v_.position, "Mesh vertex data error, expected: {}, got: {}", v.position, v_.position);
        // }
        // for (int i = 0; i < 6; i++)
        // {
        //     GE_CORE_ASSERT(mesh.indices[i] == loaded_mesh.indices[i],
        //                    "Mesh index data error, expected: {}, got: {}",
        //                    mesh.indices[i],
        //                    loaded_mesh.indices[i]);
        // }
    }
} // namespace GE