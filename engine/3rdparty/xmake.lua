-------------------------------- spdlog --------------------------------
package("GE_spdlog")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "spdlog"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
    on_test(function (package)
    end)

function GE_link_spdlog() 
    add_includedirs("engine/3rdparty/spdlog/include")
end

-------------------------------- glm --------------------------------
package("GE_glm")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "glm"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
    on_test(function (package)
    end)

function GE_link_glm() 
    add_includedirs("engine/3rdparty/glm")
end

-------------------------------- glfw --------------------------------
function GE_link_glfw() 
    if is_plat("windows") then 
        add_includedirs("engine/3rdparty/glfw/Windows/include")
        add_links("engine/3rdparty/glfw/Windows/glfw3dll")
    end
end
