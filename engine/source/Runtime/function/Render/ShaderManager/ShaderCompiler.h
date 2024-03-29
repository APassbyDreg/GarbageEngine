#pragma once

#include "GE_pch.h"

#include "ShaderModule.h"

#include "Runtime/resource/Managers/ResourceManager.h"
#include "Runtime/resource/ResourceTypes/TextResource.h"

namespace GE
{
    class ShaderCompiler
    {
    public:
        ShaderCompiler(ShaderType               type,
                       std::vector<std::string> include_dirs = {},
                       std::vector<std::string> defines      = {}) :
            m_type(type),
            m_includeDirs(include_dirs), m_defines(defines)
        {
            m_includeDirs.push_back(Config::shader_dir);
        };

        virtual std::unique_ptr<ShaderModule> Compile(std::string path, std::string entry) = 0;

        inline void AddIncludeDir(std::string dir) { m_includeDirs.push_back(dir); }
        inline void AddDefine(std::string define) { m_defines.push_back(define); }

        ShaderType               m_type;
        std::vector<std::string> m_includeDirs;
        std::vector<std::string> m_defines;
    };
} // namespace GE