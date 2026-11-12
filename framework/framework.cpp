#include "framework.h"
#include "config.h"

#include <exception>

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>

#define USER_POINTER() static_cast<App *>(glfwGetWindowUserPointer(window))

namespace framework {

static void OnErrorCallback(int error, const char *description) { SPDLOG_ERROR("({}) {}", error, description); }

static void OnFramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    USER_POINTER()->GetContext()->framebuffer_size = glm::ivec2(width, height);
    USER_POINTER()->OnFramebufferSizeCallback(width, height);
}

static void OnKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    USER_POINTER()->OnKeyCallback(key, scancode, action, mods);
}

static void OnMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    USER_POINTER()->OnMouseButtonCallback(button, action, mods);
}

static void OnCursorCallback(GLFWwindow *window, double x, double y) { USER_POINTER()->OnCursorCallback(x, y); }

static void OnScrollCallback(GLFWwindow *window, double dx, double dy) { USER_POINTER()->OnScrollCallback(dx, dy); }

void App::Init(Context *context) { m_context = context; }

void App::OnFramebufferSizeCallback(int width, int height) {}

void App::OnKeyCallback(int key, int scancode, int action, int mods) {}

void App::OnMouseButtonCallback(int button, int action, int mods) {}

void App::OnCursorCallback(double x, double y) {}

void App::OnScrollCallback(double dx, double dy) {}

Runner::Runner(std::unique_ptr<App> app, const char *title) : m_app(std::move(app)) {
    Init(title, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void Runner::Init(const char *title, int width, int height) {
    glfwSetErrorCallback(OnErrorCallback);
    if (!glfwInit()) {
        SPDLOG_CRITICAL("Failed to initialize GLFW");
        std::terminate();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLSL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLSL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    float scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        SPDLOG_CRITICAL("Failed to create window");
        std::terminate();
    }
    glfwGetFramebufferSize(m_window, &m_context.framebuffer_size.x, &m_context.framebuffer_size.y);

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    gladLoaderLoadGL();

    glfwSetWindowUserPointer(m_window, m_app.get());
    glfwSetFramebufferSizeCallback(m_window, OnFramebufferSizeCallback);
    glfwSetKeyCallback(m_window, OnKeyCallback);
    glfwSetMouseButtonCallback(m_window, OnMouseButtonCallback);
    glfwSetCursorPosCallback(m_window, OnCursorCallback);
    glfwSetScrollCallback(m_window, OnScrollCallback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION_STRING);
}

Runner::~Runner() {
    m_app.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Runner::Run() {
    m_app->Init(&m_context);

    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        m_app->Render();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window);
    }
}

} // namespace framework
