#pragma once

#include "GE_pch.h"

#include "ShaderCompiler.h"

#include "shaderc/shaderc.hpp"

namespace GE
{
    class GLSLShaderIncluder : public shaderc::CompileOptions::IncluderInterface
    {
    public:
        GLSLShaderIncluder(std::string _shader_path);
        GLSLShaderIncluder(std::string _shader_path, std::vector<std::string> additional_include_dirs);
        virtual ~GLSLShaderIncluder() = default;

        void AddIncludePath(std::string path) { m_localIncludePaths.push_back(path); };

        virtual shaderc_include_result* GetInclude(const char*          requested_source,
                                                   shaderc_include_type type,
                                                   const char*          requesting_source,
                                                   size_t               include_depth) override;

        virtual void ReleaseInclude(shaderc_include_result* data) override;

    private:
        std::string __resolve_path(std::string path, bool relative);

        fs::path                     m_shaderPath;
        std::vector<fs::path>        m_localIncludePaths;
        static std::vector<fs::path> s_globalIncludePaths;
    };

    class GLSLCompiler : public ShaderCompiler
    {
    public:
        GLSLCompiler(ShaderType               type,
                     std::vector<std::string> include_dirs = {},
                     std::vector<std::string> defines      = {}) :
            ShaderCompiler(type, include_dirs, defines) {};

        std::shared_ptr<ShaderModule> Compile(std::string shader_path) override;

    private:
        shaderc::Compiler m_compiler;
    };
} // namespace GE