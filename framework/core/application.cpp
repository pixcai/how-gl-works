#include "framework.h"
#include "framework/utils/logger.h"

#include <memory>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define USER_POINTER() static_cast<Application *>(glfwGetWindowUserPointer(window))

namespace Framework {

static void errorCallback(int error, const char *description) { logError("({}) {}", error, description); }

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    USER_POINTER()->onFramebufferSizeCallback(width, height);
}

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    USER_POINTER()->onMouseButtonCallback(button, action, mods);
}

static void cursorPosCallback(GLFWwindow *window, double x, double y) { USER_POINTER()->onCursorPosCallback(x, y); }

static void scrollCallback(GLFWwindow *window, double dx, double dy) { USER_POINTER()->onScrollCallback(dx, dy); }

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    USER_POINTER()->onKeyCallback(key, scancode, action, mods);
}

#if defined(__APPLE__)
Application::Application(const char *title) : Application(title, {1200, 800, 4, 4, 1}) {}
#else
Application::Application(const char *title) : Application(title, {1920, 1080, 4, 4, 5}) {}
#endif

Application::Application(const char *title, const ApplicationOptions &options) : options(options) {
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) {
        throw std::runtime_error("Error: Failed to initialize GLFW");
    }
    logInfo("OpenGL version: {}.{}", options.glsl_major, options.glsl_minor);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, options.glsl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, options.glsl_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    float scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    window = std::make_unique<Window>(options.width, options.height, title);

    GLFWwindow *handle = window->handle();
    glfwMakeContextCurrent(handle);
    gladLoaderLoadGL();
    glfwSwapInterval(1);

    glfwSetWindowUserPointer(handle, this);
    glfwSetFramebufferSizeCallback(handle, framebufferSizeCallback);
    glfwSetMouseButtonCallback(handle, mouseButtonCallback);
    glfwSetCursorPosCallback(handle, cursorPosCallback);
    glfwSetScrollCallback(handle, scrollCallback);
    glfwSetKeyCallback(handle, keyCallback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;

    ImGui_ImplGlfw_InitForOpenGL(handle, true);
    ImGui_ImplOpenGL3_Init(std::format("#version {}", options.glsl_major * 100 + options.glsl_minor * 10).c_str());
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    window.reset();
    glfwTerminate();
}

void Application::run() {
    GLFWwindow *handle = window->handle();

    while (!glfwWindowShouldClose(handle)) {
        glfwPollEvents();
        if (glfwGetWindowAttrib(handle, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        render();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(handle);
    }
}

void Application::onFramebufferSizeCallback(int width, int height) {}

void Application::onMouseButtonCallback(int button, int action, int mods) {}

void Application::onCursorPosCallback(double x, double y) {}

void Application::onScrollCallback(double dx, double dy) {}

void Application::onKeyCallback(int key, int scancode, int action, int mods) {}

} // namespace Framework
