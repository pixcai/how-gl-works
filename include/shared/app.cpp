#include "app.h"
#include "config.h"
#include "gpu_buffer.h"
#include "gpu_object.h"

#include <cstdlib>
#include <exception>
#include <utility>

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <memory>
#include <spdlog/spdlog.h>

#if defined(__APPLE__)
static constexpr int kWidth  = 1280;
static constexpr int kHeight = 720;
#else
static constexpr int kWidth  = 1920;
static constexpr int kHeight = 1080;
#endif

static void OnError(int code, const char *description) { spdlog::error("GLFW Error({}): {}", code, description); }

App::App(const char *title)
    : App(title, kWidth, kHeight) {}

App::App(const char *title, int width, int height) {
    spdlog::set_level(spdlog::level::trace);
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_shader_manager.Set("mesh", {
                                     {GL_VERTEX_SHADER,   "mesh.vert"},
                                     {GL_FRAGMENT_SHADER, "mesh.frag"}
    });
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

    auto M            = std::make_unique<Context>();
    M->event_manager  = &m_event_manager;
    M->shader_manager = &m_shader_manager;
    M->draw_buffer    = std::make_unique<GPUDrawBuffer>(g_mesh_layout);

    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    M->camera_controller.camera.SetPerspective(45.0f, static_cast<float>(width) / height, 0.1f, 100.0f);

    try {
        main_loop.M = std::move(M);
        main_loop.Init();

        while (!glfwWindowShouldClose(m_window)) {
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            glClearColor(0.1451f, 0.1647f, 0.1961f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            main_loop.M->draw_buffer->BeginFrame();
            main_loop.Render();
            main_loop.M->draw_buffer->EndFrame();

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
