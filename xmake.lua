set_languages("cxx23")

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "$(builddir)"})

add_requires("imgui", {configs = {opengl3 = true, glfw = true}})
add_requires("glad", {configs = {profile = "core"}})
add_requires("glfw", "glm", "stb", "fastgltf", "spdlog")

add_packages("imgui", "glad", "glm", "spdlog", {public = true})

add_includedirs("$(projectdir)", {public = true})

add_defines('PROJECT_ROOT="' .. path.unix(os.projectdir()) .. '"', {public = true})
if is_mode("debug") then
    add_defines("SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE", {public = true})
else
    add_defines("SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO", {public = true})
end

target("framework")
    set_kind("static")
    add_files("framework/**.cpp")
    add_packages("glfw", "stb", "fastgltf")
    add_defines("GLFW_INCLUDE_NONE")

includes("samples/**/xmake.lua")
