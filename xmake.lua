set_languages("cxx23")

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "$(builddir)"})

add_requires("imgui", {configs = {opengl3 = true, glfw = true}})
add_requires("glad", {configs = {profile = "core"}})
add_requires("glfw", "glm", "stb", "fastgltf")

add_includedirs("$(projectdir)", {public = true})

add_defines('PROJECT_ROOT="' .. path.unix(os.projectdir()) .. '"', {public = true})
add_defines("GLFW_INCLUDE_NONE", {public = true})

target("framework")
    set_kind("static")
    add_files("framework/**.cpp")
    add_packages("imgui", "glad", "glfw", "glm", "fastgltf", {public = true})

includes("samples/**/xmake.lua")
