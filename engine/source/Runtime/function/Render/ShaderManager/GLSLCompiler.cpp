#include "GLSLCompiler.h"

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

    inline shaderc_shader_kind __shadertype2kind(ShaderType type)
    {
        switch (type)
        {
            case ShaderType::VERTEX:
                return shaderc_vertex_shader;
            case ShaderType::TESSELLATION_CONTROL:
                return shaderc_tess_control_shader;
            case ShaderType::TESSELLATION_EVALUATION:
                return shaderc_tess_evaluation_shader;
            case ShaderType::GEOMETRY:
                return shaderc_geometry_shader;
            case ShaderType::FRAGMENT:
                return shaderc_fragment_shader;
            case ShaderType::COMPUTE:
                return shaderc_compute_shader;
            case ShaderType::MESH:
                return shaderc_mesh_shader;
            case ShaderType::TASK:
                return shaderc_task_shader;
            case ShaderType::RAY_GENERATION:
                return shaderc_raygen_shader;
            case ShaderType::RAY_INTERSECTION:
                return shaderc_intersection_shader;
            case ShaderType::RAY_ANYHIT:
                return shaderc_anyhit_shader;
            case ShaderType::RAY_CLOSESTHIT:
                return shaderc_closesthit_shader;
            case ShaderType::RAY_MISS:
                return shaderc_miss_shader;
            case ShaderType::RAY_CALLABLE:
                return shaderc_callable_shader;
            default:
                return shaderc_glsl_infer_from_source;
        };
    }

    /* --------------------------- GLSLShaderIncluder --------------------------- */

    std::vector<fs::path> GLSLShaderIncluder::s_globalIncludePaths = {Config::shader_dir};

    GLSLShaderIncluder::GLSLShaderIncluder(std::string _shader_path) : m_shaderPath(_shader_path) {}

    GLSLShaderIncluder::GLSLShaderIncluder(std::string _shader_path, std::vector<std::string> additional_include_dirs) :
        m_shaderPath(_shader_path)
    {
        for (auto&& d : additional_include_dirs)
        {
            m_localIncludePaths.push_back(d);
        }
    }

    std::string GLSLShaderIncluder::__resolve_path(std::string basepath, bool relative)
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
            GE_CORE_ERROR("GLSLShaderIncluder: could not resolve path: {} [{}]", basepath, msg);
            return "";
        }
    }

    shaderc_include_result* GLSLShaderIncluder::GetInclude(const char*          requested_source,
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

    void GLSLShaderIncluder::ReleaseInclude(shaderc_include_result* data)
    {
        delete static_cast<std::array<std::string, 2>*>(data->user_data);
        delete data;
    };

    /* ---------------------------- GLSL Compiler --------------------------- */

    std::shared_ptr<ShaderModule> GLSLCompiler::Compile(std::string path)
    {
        std::string ext = fs::path(path).extension().string();

        shaderc_shader_kind shader_kind     = __shadertype2kind(m_type);
        std::string         shader_filename = fs::path(path).filename().string();
        std::string         shader_src_code = __try_load_shader_str(path);
        GE_CORE_ASSERT(shader_src_code.length() > 0, "Failed to load shader file: {}", path);

        shaderc::CompileOptions options;
        options.SetIncluder(std::make_unique<GLSLShaderIncluder>(path, m_includeDirs));
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
        options.SetGenerateDebugInfo();
        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        auto result = m_compiler.CompileGlslToSpv(shader_src_code, shader_kind, shader_filename.c_str(), options);
        GE_CORE_ASSERT(result.GetCompilationStatus() == shaderc_compilation_status_success,
                       "Failed to compile shader[{}]: {}",
                       (int)result.GetCompilationStatus(),
                       result.GetErrorMessage());
        auto spv = std::vector<uint32_t>(result.cbegin(), result.cend());

        return std::make_shared<ShaderModule>(spv, m_type);
    }
} // namespace GE