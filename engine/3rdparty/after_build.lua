local function glfw(destdir)
    local destfile = path.join(destdir, "glfw3.dll")
    if not os.exists(destfile) then
        local srcfile = path.join(os.scriptdir(), "glfw/lib/Windows/lib-vc2022/glfw3.dll")
        os.cp(srcfile, destfile)
        print("copy %s to %s", srcfile, destfile)
    end
end

function after_build(build_dir)
    glfw(build_dir)
end