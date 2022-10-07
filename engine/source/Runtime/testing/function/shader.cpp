#include "shader.h"

namespace GE
{
    void ShaderTestCase::run()
    {
        {
            fs::path     fullpath   = fs::path(Config::shader_dir) / "Passes/__test02_simple_mesh/test.hlsl";
            HLSLCompiler vscompiler = {ShaderType::VERTEX};
            HLSLCompiler fscompiler = {ShaderType::FRAGMENT};
            vscompiler.Compile(fullpath.string(), "vert");
            fscompiler.Compile(fullpath.string(), "frag");
        }
        {
            fs::path     fullpath   = fs::path(Config::shader_dir) / "Passes/__test02_simple_mesh/test.frag";
            GLSLCompiler fscompiler = {ShaderType::FRAGMENT};
            fscompiler.Compile(fullpath.string(), "frag");
        }
    }
} // namespace GE