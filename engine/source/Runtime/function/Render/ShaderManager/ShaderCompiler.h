#pragma once

#include "GE_pch.h"

#include "ShaderModule.h"

namespace GE
{
    class ShaderCompiler
    {
    public:
        ShaderCompiler(ShaderType               type,
                       std::vector<std::string> include_dirs = {},
                       std::vector<std::string> defines      = {}) :
            m_type(type),
            m_includeDirs(include_dirs), m_defines(defines) {};

        virtual std::shared_ptr<ShaderModule> Compile(std::string path, std::string entry) = 0;

        ShaderType               m_type;
        std::vector<std::string> m_includeDirs;
        std::vector<std::string> m_defines;
    };
} // namespace GE