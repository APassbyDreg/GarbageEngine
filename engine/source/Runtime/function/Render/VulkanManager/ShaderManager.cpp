#include "ShaderManager.h"

#include "Runtime/function/Log/LogSystem.h"
#include "Runtime/function/Render/VulkanManager/VulkanManager.h"

#include "Runtime/core/Hash.h"

namespace GE
{
    std::string __try_load_shader_str(const std::string& path)
    {
        if (fs::exists(path))
        {
            std::ifstream     file(path);
            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }
        return "";
    }

    /* --------------------------- ShaderIncluder --------------------------- */

    std::vector<fs::path> ShaderIncluder::s_globalIncludePaths = {Config::shader_dir};

    ShaderIncluder::ShaderIncluder(std::string _shader_path) : m_shaderPath(_shader_path) {}

    ShaderIncluder::ShaderIncluder(std::string _shader_path, std::vector<std::string> additional_include_dirs) :
        m_shaderPath(_shader_path)
    {
        for (auto&& d : additional_include_dirs)
        {
            m_localIncludePaths.push_back(d);
        }
    }

    std::string ShaderIncluder::__resolve_path(std::string basepath, bool relative)
    {
        fs::path resolved_path;
        fs::path path;
        uint     match = 0;
        if (relative)
        {
            path = m_shaderPath.parent_path() / basepath;
            if (fs::exists(path))
            {
                match += 1;
                resolved_path = path;
            }
        }
        for (auto&& include_dir : m_localIncludePaths)
        {
            path = include_dir / basepath;
            if (fs::exists(path))
            {
                match += 1;
                resolved_path = path;
            }
        }
        for (auto&& include_dir : s_globalIncludePaths)
        {
            path = include_dir / basepath;
            if (fs::exists(path))
            {
                match += 1;
                resolved_path = path;
            }
        }

        if (match == 1)
        {
            return resolved_path.string();
        }
        else
        {
            std::string msg = (match == 0) ? "No file found" : "Multiple files found";
            GE_CORE_ERROR("ShaderIncluder: could not resolve path: {} [{}]", basepath, msg);
            return "";
        }
    }

    shaderc_include_result* ShaderIncluder::GetInclude(const char*          requested_source,
                                                       shaderc_include_type type,
                                                       const char*          requesting_source,
                                                       size_t               include_depth)
    {
        std::string path, contents;

        path = __resolve_path(std::string(requested_source), type == shaderc_include_type_relative);
        GE_CORE_ASSERT(path.length() > 0, "Failed to resolve include path: {}", requested_source);
        contents = __try_load_shader_str(path);
        GE_CORE_ASSERT(contents.length() > 0, "Failed to load include file: {}", path);

        auto container  = new std::array<std::string, 2>;
        (*container)[0] = path;
        (*container)[1] = contents;

        auto data                = new shaderc_include_result;
        data->user_data          = container;
        data->source_name        = (*container)[0].data();
        data->source_name_length = (*container)[0].size();
        data->content            = (*container)[1].data();
        data->content_length     = (*container)[1].size();

        return data;
    }

    void ShaderIncluder::ReleaseInclude(shaderc_include_result* data)
    {
        delete static_cast<std::array<std::string, 2>*>(data->user_data);
        delete data;
    };

    /* ---------------------------- ShaderModule ---------------------------- */

    ShaderModule::ShaderModule(std::vector<uint32_t> spv)
    {
        VkShaderModuleCreateInfo create_info = {};
        create_info.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize                 = spv.size() * sizeof(uint32_t);
        create_info.pCode                    = spv.data();

        VK_CHECK(vkCreateShaderModule(VulkanManager::GetInstance().GetVkDevice(), &create_info, nullptr, &m_module));
    }

    ShaderModule::~ShaderModule()
    {
        if (m_compiled)
        {
            vkDestroyShaderModule(VulkanManager::GetInstance().GetVkDevice(), m_module, nullptr);
        }
    }

    /* ------------------------------ ShaderManager ----------------------------- */

    shaderc_shader_kind ShaderManager::__shader_kind_from_ext(std::string filepath)
    {
        const std::map<std::string, shaderc_shader_kind> ext2kind = {
            {".vert", shaderc_vertex_shader},
            {".tesc", shaderc_tess_control_shader},
            {".tese", shaderc_tess_evaluation_shader},
            {".geom", shaderc_geometry_shader},
            {".frag", shaderc_fragment_shader},
            {".comp", shaderc_compute_shader},
            {".mesh", shaderc_mesh_shader},
            {".task", shaderc_task_shader},
            {".rgen", shaderc_raygen_shader},
            {".rint", shaderc_intersection_shader},
            {".rahit", shaderc_anyhit_shader},
            {".rchit", shaderc_closesthit_shader},
            {".rmiss", shaderc_miss_shader},
            {".rcall", shaderc_callable_shader},
        };
        std::string ext = fs::path(filepath).extension().string();
        auto        it  = ext2kind.find(ext);
        if (it != ext2kind.end())
        {
            return it->second;
        }
        else
        {
            return shaderc_glsl_infer_from_source;
        }
    }

    ShaderModule ShaderManager::GetCompiledModule(std::string              path,
                                                  std::vector<std::string> additional_include_dirs,
                                                  bool&                    use_cache)
    {
        shaderc::CompileOptions options;
        options.SetIncluder(std::make_unique<ShaderIncluder>(path, additional_include_dirs));
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
        options.SetGenerateDebugInfo();
        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        std::vector<uint32_t> spv = GetCompiledSpv(path, options, use_cache);
        return ShaderModule(spv);
    }

    std::vector<uint32_t> ShaderManager::GetCompiledSpv(std::string path, shaderc::CompileOptions opt, bool& use_cache)
    {
        shaderc_shader_kind shader_kind     = __shader_kind_from_ext(path);
        std::string         shader_filename = fs::path(path).filename().string();
        std::string         shader_src_code = __try_load_shader_str(path);
        GE_CORE_ASSERT(shader_src_code.length() > 0, "Failed to load shader file: {}", path);

        /**
         * REVIEW: be careful with the shader cache.
         * I use content of shader file content as specifier, changing the includer will not invalidate old cache.
         */
        CacheDescriptor desc(path, sha256(shader_src_code));
        desc.type = "shader";
        if (use_cache)
        {
            use_cache = false;
            uint32_t* spv_data;
            uint64_t  spv_size;
            if (CacheManager::GetInstance().Load(desc, (char**)&spv_data, spv_size))
            {
                use_cache = true;
                GE_CORE_TRACE("Loaded shader from cache: {}", path);
                std::vector<uint32_t> spv = std::vector<uint32_t>(spv_data, spv_data + spv_size / sizeof(uint32_t));
                delete[] spv_data;
                return spv;
            }
        }

        auto result = m_compiler.CompileGlslToSpv(shader_src_code, shader_kind, shader_filename.c_str(), opt);
        GE_CORE_ASSERT(result.GetCompilationStatus() == shaderc_compilation_status_success,
                       "Failed to compile shader[{}]: {}",
                       (int)result.GetCompilationStatus(),
                       result.GetErrorMessage());

        // save to cache
        uint64_t spv_size = (result.cend() - result.cbegin()) * sizeof(uint32_t);
        CacheManager::GetInstance().Save(desc, (char*)result.cbegin(), spv_size);

        return std::vector<uint32_t>(result.cbegin(), result.cend());
    }

    void ShaderManager::ClearShaderCache()
    {
        uint cnt = CacheManager::GetInstance().Invalidate(
            [](std::string key, std::string type, std::string specifier) { return type.starts_with("shader"); });
        GE_CORE_TRACE("Cleared {} shader cache items", cnt);
    }
} // namespace GE