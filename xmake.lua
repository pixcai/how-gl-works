add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "$(builddir)"})

set_languages("cxx23")
add_requires("imgui", {configs = {opengl3 = true, glfw = true}})
add_requires("glad", {configs = {profile = "core"}})
add_requires("glfw", "glm", "stb", "fastgltf")

target("framework")
    set_kind("static")
    add_files("framework/**.cpp")
    add_includedirs("framework", {public = true})
    add_defines("GLFW_INCLUDE_NONE", {public = true})
    add_packages("imgui", "glad", "glfw", "glm", {public = true})

includes("samples/**/xmake.lua")
