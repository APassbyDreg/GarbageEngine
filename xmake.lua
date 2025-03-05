add_rules("mode.debug", "mode.release", "mode.profile")

set_project("GarbageEngine")

includes("engine/3rdparty")
GE_load_3rdparty()


function GE_add_predefined_macros() 
    if is_mode("debug") then
        add_defines("GE_DEBUG")
    end

    if is_plat("windows") then
        add_defines("GE_PLATFORM_WINDOWS")
    end
    -- add_defines("GE_ENABLE_TESTING")
end

-- runtime
target("runtime")
    set_languages("c++20")
    set_kind("static")

    -- set_warnings("all", "extra")
    -- add_cxxflags("/W4")

    -- defines
    GE_add_predefined_macros()
    add_defines("GE_BUILD_RUNTIME")

    -- precompiled header
    -- set_pcxxheader("engine/source/GE_pch.h")

    -- linking packages
    GE_link_3rdparty()
    add_includedirs("engine/source")

    -- files
    add_files("engine/source/runtime/**.cpp")
    add_headerfiles("engine/source/runtime/**.h")

    set_configvar("GE_ASSET_DIR", path.join(os.scriptdir(), "engine/assets"), {escape = true})
    set_configvar("GE_SHADER_DIR", path.join(os.scriptdir(), "engine/shaders"), {escape = true})
    set_configdir(path.join(os.scriptdir(), "engine/source/Runtime/config"))
    add_configfiles("engine/source/runtime/**.in")


-- editor
target("editor")
    set_languages("c++20")
    set_kind("binary")

    -- defines
    GE_add_predefined_macros()
    add_defines("GE_BUILD_EDITOR")

    -- dependencies
    add_deps("runtime")

    -- precompiled header
    -- set_pcxxheader("engine/source/GE_pch.h")

    -- linking packages
    GE_link_3rdparty()
    add_includedirs("engine/source", "engine/source/Editor")

    -- files
    add_files("engine/source/editor/**.cpp")
    add_headerfiles("engine/source/editor/**.h")

    -- after build action
    after_build(function (target)
        import("core.project.config")
        local targetfile = target:targetfile()
        local buildir = path.directory(targetfile)

        import("engine.3rdparty.after_build", {alias = "ab"})
        ab.after_build(buildir)
    end)