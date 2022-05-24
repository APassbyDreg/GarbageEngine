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
package("GE_glfw")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "glfw"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
    on_test(function (package)
    end)

function GE_require_glfw()
    add_requires("GE_glfw")
end

function GE_link_glfw() 
    add_packages("GE_glfw")
end
