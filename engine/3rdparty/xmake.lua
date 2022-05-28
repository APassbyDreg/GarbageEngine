local function rel_local_path(relpath)
    local dir = os.scriptdir()
    if not dir:endswith("3rdparty") then 
        dir = path.join(dir, "engine/3rdparty")
    end
    return path.join(dir, relpath)
end

-------------------------------- spdlog --------------------------------
function GE_link_spdlog()
    add_includedirs(rel_local_path("spdlog/include"))
end


-------------------------------- glm --------------------------------
function GE_link_glm() 
    add_includedirs(rel_local_path("glm"))
end


-------------------------------- glfw --------------------------------
function GE_link_glfw() 
    if is_plat("windows") then 
        add_includedirs(rel_local_path("glfw/Windows/include"))
        add_links(rel_local_path("glfw/Windows/glfw3dll"))
    end
end


-------------------------------- vulkan
--------------------------------
function GE_link_vulkan() 
    add_includedirs(rel_local_path("vulkan/include"))
    if is_plat("windows") then 
        add_links(rel_local_path("vulkan/lib/Windows/vulkan-1"))
    end
end


-------------------------------- imgui --------------------------------
target("GE_imgui")
    set_kind("static")

    GE_link_vulkan()
    GE_link_glfw()
    add_includedirs(rel_local_path("imgui"))

    add_files("imgui/*.cpp")
    add_files("imgui/backends/imgui_impl_glfw.cpp")
    add_files("imgui/backends/imgui_impl_vulkan.cpp")


function GE_link_imgui() 
    add_includedirs(rel_local_path("imgui"))
    add_deps("GE_imgui") 
end


-------------------------------- vma --------------------------------
function GE_link_vma() 
    add_includedirs(rel_local_path("VulkanMemoryAllocator-Hpp/include"))
end


-------------------------------- vkb --------------------------------
target("GE_vkb")
    set_kind("static")

    GE_link_vulkan()
    add_includedirs(rel_local_path("vk-bootstrap/src"))

    add_files(rel_local_path("vk-bootstrap/src/**.cpp"))

function GE_link_vkb() 
    add_includedirs(rel_local_path("vk-bootstrap/src"))
    add_deps("GE_vkb")
end


-------------------------------- json --------------------------------
function GE_link_json() 
    add_includedirs(rel_local_path("nlohmann_json/single_include"))
end


-------------------------------- overall --------------------------------
function GE_link_3rdparty()
    GE_link_vulkan() 
    GE_link_glfw()
    GE_link_glm()
    GE_link_spdlog()
    GE_link_imgui()
    GE_link_vma()
    GE_link_vkb() 
    GE_link_json()
end


function GE_load_3rdparty() 
end