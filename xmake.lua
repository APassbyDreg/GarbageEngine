add_rules("mode.debug", "mode.release")

set_project("GarbageEngine")

includes("engine/3rdparty")
GE_load_3rdparty()


function GE_add_predefined_macros() 
    is_mode("debug")
        add_defines("GE_DEBUG")
    
    is_plat("windows")
        add_defines("GE_PLATFORM_WINDOWS")

    add_defines("GE_ENABLE_TESTING")
end

-- runtime
target("runtime")
    set_languages("c++20")
    set_kind("shared")

    -- defines
    GE_add_predefined_macros()
    add_defines("GE_BUILD_RUNTIME")

    -- precompiled header
    set_pcxxheader("engine/source/GE_pch.h")

    -- linking packages
    GE_link_3rdparty()
    add_includedirs("engine/source", "engine/source/Runtime")

    -- files
    add_files("engine/source/runtime/**.cpp")


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
    set_pcxxheader("engine/source/GE_pch.h")

    -- linking packages
    GE_link_3rdparty()
    add_includedirs("engine/source", "engine/source/Editor")

    -- files
    add_files("engine/source/editor/**.cpp")

    -- after build action
    after_build(function (target)
        import("core.project.config")
        local targetfile = target:targetfile()
        local buildir = path.directory(targetfile)

        import("engine.3rdparty.after_build", {alias = "ab"})
        ab.after_build(buildir)
    end)