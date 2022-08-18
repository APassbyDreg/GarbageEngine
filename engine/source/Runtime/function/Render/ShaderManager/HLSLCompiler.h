#pragma once

#include "GE_pch.h"

#include "ShaderCompiler.h"

namespace GE
{
    class HLSLCompiler : public ShaderCompiler
    {
    public:
        HLSLCompiler(ShaderType               type,
                     std::vector<std::string> include_dirs = {},
                     std::vector<std::string> defines      = {}) :
            ShaderCompiler(type, include_dirs, defines) {};

        std::shared_ptr<ShaderModule> Compile(std::string shader_path) override;
    };
} // namespace GE