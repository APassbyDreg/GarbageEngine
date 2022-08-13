local function rel_local_path(relpath)
    local dir = os.scriptdir()
    if not dir:endswith("3rdparty") then 
        dir = path.join(dir, "engine/3rdparty")
    end
    return path.join(dir, relpath)
end

-------------------------------- spdlog --------------------------------
function GE_link_spdlog()
    add_defines("SPDLOG_USE_STD_FORMAT") -- this must be added to avoid compile error in c++20
    add_includedirs(rel_local_path("spdlog/include"))
end


-------------------------------- glm --------------------------------
function GE_link_glm() 
    add_includedirs(rel_local_path("glm"))
end


-------------------------------- glfw --------------------------------
function GE_link_glfw() 
    add_includedirs(rel_local_path("glfw/include"))
    if is_plat("windows") then
        add_links(rel_local_path("glfw/lib/Windows/lib-vc2022/glfw3dll"))
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
    add_includedirs(rel_local_path("imgui/backends"))
    add_deps("GE_imgui") 
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


-------------------------------- entt --------------------------------
function GE_link_entt() 
    add_includedirs(rel_local_path("entt/single_include"))
end


-------------------------------- shaderc --------------------------------
function GE_link_shaderc() 
    add_includedirs(rel_local_path("shaderc/include"))
    add_links(rel_local_path("shaderc/lib/shaderc_combined"))
end


-------------------------------- miniz --------------------------------
target("GE_miniz")
    set_kind("static")

    add_includedirs(rel_local_path("miniz"))

    add_files(rel_local_path("miniz/**.cpp"))

function GE_link_miniz() 
    add_includedirs(rel_local_path("miniz"))
    add_deps("GE_miniz")
end


-------------------------------- stb --------------------------------
function GE_link_stb() 
    add_includedirs(rel_local_path("stb"))
end


-------------------------------- dxc --------------------------------
function GE_link_dxc() 
    add_includedirs(rel_local_path("dxc/include"))
    
    if is_plat("windows") then 
        add_links(rel_local_path("dxc/lib/dxclib"))
        add_links(rel_local_path("dxc/lib/dxcompiler"))
    end
end


-------------------------------- overall --------------------------------
function GE_link_3rdparty()
    GE_link_vulkan()
    GE_link_glfw()
    GE_link_glm()
    GE_link_spdlog()
    GE_link_imgui()
    GE_link_vkb()
    GE_link_json()
    GE_link_entt()
    GE_link_shaderc()
    GE_link_miniz()
    GE_link_stb()

    add_packages("openssl")
end


function GE_load_3rdparty()
    add_requires("openssl")
end