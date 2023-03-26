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

        std::unique_ptr<ShaderModule> Compile(std::string path, std::string entry) override;

    private:
        std::vector<std::wstring> __setup_compile_args(std::string path, std::string entry);
    };
} // namespace GE