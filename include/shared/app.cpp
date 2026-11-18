#include "app.h"
#include "config.h"

#include <cstdlib>
#include <exception>

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>

#if defined(__APPLE__)
static constexpr int kWidth  = 1280;
static constexpr int kHeight = 720;
#else
static constexpr int kWidth  = 1920;
static constexpr int kHeight = 1080;
#endif

static void OnError(int code, const char *description) { spdlog::error("GLFW Error({}): {}", code, description); }

App::App(const char *title) : App(title, kWidth, kHeight) {}

App::App(const char *title, int width, int height) {
    glfwSetErrorCallback(OnError);
    if (!glfwInit()) {
        spdlog::critical("Failed to initialize GLFW");
        std::terminate();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, kGLVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, kGLVersionMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        spdlog::critical("Failed to create window");
        glfwTerminate();
        std::terminate();
    }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    gladLoaderLoadGL();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    float scale       = ImGui_ImplGlfw_GetContentScaleForWindow(m_window);
    ImGuiStyle &style = ImGui::GetStyle();

    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(kGLSLVersionString);
}

App::~App() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

int App::Run(MainLoop &&main_loop) {
    int exit_code = EXIT_SUCCESS;

    try {
        while (!glfwWindowShouldClose(m_window)) {
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            glClearColor(0.1451f, 0.1647f, 0.1961f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            main_loop.Render();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(m_window);
        }
    } catch (const std::exception &e) {
        spdlog::critical(e.what());
        exit_code = EXIT_FAILURE;
    }

    return exit_code;
}
