function glfw(destdir)
    local dll = path.join(os.scriptdir(), "glfw/Windows/glfw3.dll")
    os.cp(dll, path.join(destdir, "glfw3.dll"))
    print("copy %s to %s", dll, path.join(destdir, "glfw3.dll"))
end