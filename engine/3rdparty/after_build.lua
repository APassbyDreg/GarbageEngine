function glfw(destdir)
    local destfile = path.join(destdir, "glfw3.dll")
    if not os.exists(destfile) then
        local srcfile = path.join(os.scriptdir(), "glfw/Windows/glfw3.dll")
        os.cp(srcfile, path.join(destdir, "glfw3.dll"))
        print("copy %s to %s", srcfile, destfile)
    end
end