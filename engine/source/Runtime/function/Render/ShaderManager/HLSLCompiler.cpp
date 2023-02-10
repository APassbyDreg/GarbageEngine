#include "HLSLCompiler.h"

#include "dxc/dxcapi.h"

namespace GE
{
    std::shared_ptr<ShaderModule> HLSLCompiler::Compile(std::string shader_path, std::string entry)
    {
        // Create compiler and utils.
        CComPtr<IDxcUtils>     pUtils;
        CComPtr<IDxcCompiler3> pCompiler;
        DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&pUtils));
        DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler));

        // Create default include handler.
        CComPtr<IDxcIncludeHandler> pIncludeHandler;
        pUtils->CreateDefaultIncludeHandler(&pIncludeHandler);

        // Command line args.
        std::vector<std::wstring> args = __setup_compile_args(shader_path, entry);
        std::vector<LPCWSTR>      pszArgs;
        for (auto& arg : args)
        {
            pszArgs.push_back(arg.c_str());
        }

        // Open source file.
        CComPtr<IDxcBlobEncoding> pSource       = nullptr;
        std::wstring              shader_path_w = std::wstring(shader_path.begin(), shader_path.end());
        pUtils->LoadFile(shader_path_w.c_str(), nullptr, &pSource);
        DxcBuffer Source;
        Source.Ptr      = pSource->GetBufferPointer();
        Source.Size     = pSource->GetBufferSize();
        Source.Encoding = DXC_CP_ACP; // Assume BOM says UTF8 or UTF16 or this is ANSI text.

        // Compile it with specified arguments.
        CComPtr<IDxcResult> pResults;
        pCompiler->Compile(&Source,                // Source buffer.
                           pszArgs.data(),         // Array of pointers to arguments.
                           pszArgs.size(),         // Number of arguments.
                           pIncludeHandler,        // User-provided interface to handle #include directives (optional).
                           IID_PPV_ARGS(&pResults) // Compiler output status, buffer, and errors.
        );

        // Print errors if present.
        CComPtr<IDxcBlobUtf8> pErrors = nullptr;
        pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
        // Note that d3dcompiler would return null if no errors or warnings are present.
        // IDxcCompiler3::Compile will always return an error buffer, but its length will be zero if
        // there are no warnings or errors.
        if (pErrors != nullptr && pErrors->GetStringLength() != 0)
            GE_CORE_ERROR("HLSL Compiler: \n{}\n", pErrors->GetStringPointer());

        // Quit if the compilation failed.
        HRESULT hrStatus;
        pResults->GetStatus(&hrStatus);
        GE_CORE_ASSERT(SUCCEEDED(hrStatus), "Compilation Failed");

        // Get shader binary.
        CComPtr<IDxcBlob>      pShader     = nullptr;
        CComPtr<IDxcBlobUtf16> pShaderName = nullptr;
        pResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), &pShaderName);

        size_t                binary_size   = (pShader->GetBufferSize() + sizeof(uint32_t) - 1) / sizeof(uint32_t);
        std::vector<uint32_t> shader_binary = std::vector<uint32_t>(binary_size);
        memcpy(shader_binary.data(), pShader->GetBufferPointer(), pShader->GetBufferSize());

        return std::make_shared<ShaderModule>(shader_binary, m_type, entry);
    }

    static std::wstring __to_wstring(std::string s) { return std::wstring(s.begin(), s.end()); }

    std::vector<std::wstring> HLSLCompiler::__setup_compile_args(std::string path, std::string entry)
    {
        std::vector<std::wstring> args;
        args.emplace_back(L"-spirv");
#ifdef GE_DEBUG
        args.emplace_back(L"-Zi");
        // args.emplace_back(L"-fspv-debug=vulkan-with-source");
#endif
        args.emplace_back(L"-fspv-target-env=vulkan1.3"); // REVIEW: Use dynamic vulkan version.
        args.emplace_back(L"-E");
        args.emplace_back(std::wstring(entry.begin(), entry.end()));
        for (auto& dir : m_includeDirs)
        {
            args.emplace_back(L"-I");
            args.emplace_back(__to_wstring(dir));
        }
        for (auto& def : m_defines)
        {
            args.emplace_back(L"-D");
            args.emplace_back(__to_wstring(def));
        }
        args.emplace_back(L"-T");
        switch (m_type)
        {
            case ShaderType::VERTEX:
                args.emplace_back(L"vs_6_0");
                break;
            case ShaderType::GEOMETRY:
                args.emplace_back(L"gs_6_0");
                break;
            case ShaderType::FRAGMENT:
                args.emplace_back(L"ps_6_0");
                break;
            case ShaderType::COMPUTE:
                args.emplace_back(L"cs_6_0");
                break;
            case ShaderType::RAY_GENERATION:
            case ShaderType::RAY_INTERSECTION:
            case ShaderType::RAY_ANYHIT:
            case ShaderType::RAY_CLOSESTHIT:
            case ShaderType::RAY_MISS:
            case ShaderType::RAY_CALLABLE:
                args.emplace_back(L"lib_6_4");
                break;
            default:
                GE_CORE_CRITICAL("Unsupported hlsl shader type");
                break;
        }
        return args;
    }
} // namespace GE