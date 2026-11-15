#include "framework.h"
#include "framework/config.h"
#include "framework/context.h"
#include "framework/window.h"

#include <cstdlib>
#include <exception>
#include <stdexcept>

#define GLFW_INCLUDE_NONE 1
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>

namespace framework {

static void OnError(int error, const char *description) { spdlog::error("GLFW Error({}): {}", error, description); }

App::App(std::string title) : title(title) {
#ifdef NDEBUG
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::set_level(spdlog::level::trace);
#endif
}

int App::Run() {
    try {
        Init();
        MainLoop();
        return EXIT_SUCCESS;
    } catch (const std::exception &error) {
        spdlog::critical(error.what());
        return EXIT_FAILURE;
    }
}

void App::Init() {
    glfwSetErrorCallback(OnError);
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    window             = std::make_unique<Window>(title.c_str(), config::kWindowSize.x, config::kWindowSize.y);
    GLFWwindow *handle = window->GetHandle();

    window->on_framebuffer_resize = [&](int width, int height) { OnFramebufferResize(width, height); };
    window->on_key_pressed        = [&](int key, int scancode, int action, int mods) {
        OnKeyPressed(key, scancode, action, mods);
    };
    window->on_mouse_pressed = [&](int button, int action, int mods) { OnMousePressed(button, action, mods); };
    window->on_mouse_move    = [&](double dx, double dy) { OnMouseMove(dx, dy); };
    window->on_mouse_wheel   = [&](double dx, double dy) { OnMouseWheel(dx, dy); };

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO &io    = ImGui::GetIO();
    io.IniFilename = nullptr;

    float scale        = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    ImGuiStyle &style  = ImGui::GetStyle();
    style.FontScaleDpi = scale;
    style.ScaleAllSizes(scale);

    ImGui_ImplGlfw_InitForOpenGL(handle, true);
    ImGui_ImplOpenGL3_Init(config::kGLSLVersionString);

    context.shader_manager.Add("basic", {{GL_VERTEX_SHADER, "basic.vert"}, {GL_FRAGMENT_SHADER, "basic.frag"}});
}

void App::MainLoop() {
    GLFWwindow *handle = window->GetHandle();

    while (!glfwWindowShouldClose(handle)) {
        glfwPollEvents();
        if (glfwGetWindowAttrib(handle, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        auto clear_color = context.scene.background_color;
        glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Render();
        Window::DrawMainMenuBar(context);
        Window::DrawStatusBar(context);
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(handle);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    window->Close();
    glfwTerminate();
}

void App::OnFramebufferResize(int width, int height) {}

void App::OnKeyPressed(int key, int scancode, int action, int mods) {}

void App::OnMousePressed(int button, int action, int mods) {}

void App::OnMouseMove(double dx, double dy) {}

void App::OnMouseWheel(double dx, double dy) {}

} // namespace framework
