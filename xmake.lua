add_rules("mode.debug", "mode.release")

set_project("GarbageEngine")

includes("engine/3rdparty")

-- link vulkan libraries and include directories
vk_include = "C:/VulkanSDK/1.3.204.1/Include"
vk_lib = "C:/VulkanSDK/1.3.204.1/Lib/vulkan-1"
function GE_link_vulkan()
    add_includedirs(vk_include_dir)
    add_links(vk_lib)
end



-- runtime
target("runtime")
    set_kind("shared")

    add_defines("GE_BUILD_RUNTIME")

    GE_link_vulkan()
    GE_link_spdlog()
    GE_link_glm()
    add_includedirs("engine/source/Runtime")

    add_files("engine/source/runtime/**.cpp")


-- editor
target("editor")
    set_kind("binary")

    is_plat("windows")
        add_defines("GE_PLATFORM_WINDOWS")

    add_defines("GE_BUILD_EDITOR")

    add_deps("runtime", { inherit = true})

    GE_link_vulkan()
    GE_link_spdlog()
    GE_link_glm()
    add_includedirs("engine/source", "engine/source/Editor")

    add_files("engine/source/editor/**.cpp")