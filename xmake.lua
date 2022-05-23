add_rules("mode.debug", "mode.release")

add_requires("vcpkg::zlib")
add_requires("vcpkg::imgui")
add_requires("vcpkg::glm")
add_requires("vcpkg::spdlog")

function GE_link_vulkan()
    add_includedirs("C:/VulkanSDK/1.3.204.1/Include")
    add_linkdirs("C:/VulkanSDK/1.3.204.1/Lib")
    add_links("vulkan-1")
end

target("runtime")
    set_kind("shared")
    GE_link_vulkan()
    add_defines("GE_BUILD_RUNTIME")
    add_packages("vcpkg::zlib", "vcpkg::imgui", "vcpkg::glm", "vcpkg::eigen3", "vcpkg::spdlog")
    add_includedirs("engine/source/Runtime")
    add_files("engine/source/runtime/*.cpp")

target("editor")
    is_plat("windows")
        add_defines("GE_PLATFORM_WINDOWS")

    set_kind("binary")
    add_deps("runtime")
    add_includedirs("engine/source", "engine/source/Editor")
    add_files("engine/source/editor/*.cpp")