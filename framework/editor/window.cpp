#include "framework/config.h"
#include "framework/core/context.h"
#include "window.h"

#include <format>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <imgui.h>

#define USER_POINTER() static_cast<Window *>(glfwGetWindowUserPointer(handle))

namespace framework {

void Window::DrawMainMenuBar(Context &context) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                // Exit
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo", "Ctrl+Z");
            ImGui::MenuItem("Redo", "Ctrl+Y");
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Window::DrawStatusBar(Context &context) {
    ImGuiIO &io  = ImGui::GetIO();
    float height = 24.0f;

    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y - height));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, height));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoDocking;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));

    if (ImGui::Begin("StatusBar", nullptr, flags)) {
        auto fps_text = std::format("FPS: {:.0f}", io.Framerate);
        auto fps_size = ImGui::CalcTextSize(fps_text.c_str());
        ImGui::SameLine(ImGui::GetWindowWidth() - fps_size.x - 16);
        ImGui::Text("%s", fps_text.c_str());
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void Window::OnFramebufferResize(GLFWwindow *handle, int width, int height) {
    if (USER_POINTER()->on_framebuffer_resize) {
        USER_POINTER()->on_framebuffer_resize(width, height);
    }
}

void Window::OnKeyPressed(GLFWwindow *handle, int key, int scancode, int action, int mods) {
    if (USER_POINTER()->on_key_pressed) {
        USER_POINTER()->on_key_pressed(key, scancode, action, mods);
    }
}

void Window::OnMousePressed(GLFWwindow *handle, int button, int action, int mods) {
    if (USER_POINTER()->on_mouse_pressed) {
        USER_POINTER()->on_mouse_pressed(button, action, mods);
    }
}

void Window::OnMouseMove(GLFWwindow *handle, double dx, double dy) {
    if (USER_POINTER()->on_mouse_move) {
        USER_POINTER()->on_mouse_move(dx, dy);
    }
}

void Window::OnMouseWheel(GLFWwindow *handle, double dx, double dy) {
    if (USER_POINTER()->on_mouse_wheel) {
        USER_POINTER()->on_mouse_wheel(dx, dy);
    }
}

Window::Window(const char *title, int width, int height) {
    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config::kGLVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config::kGLVersionMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_handle) {
        throw std::runtime_error("Failed to create window");
    }
    glfwMakeContextCurrent(m_handle);
    glfwSwapInterval(1);
    gladLoaderLoadGL();

    glfwSetWindowUserPointer(m_handle, this);
    glfwSetFramebufferSizeCallback(m_handle, OnFramebufferResize);
    glfwSetKeyCallback(m_handle, OnKeyPressed);
    glfwSetMouseButtonCallback(m_handle, OnMousePressed);
    glfwSetCursorPosCallback(m_handle, OnMouseMove);
    glfwSetScrollCallback(m_handle, OnMouseWheel);

    glfwGetFramebufferSize(m_handle, &m_framebuffer_size.x, &m_framebuffer_size.y);
}

Window::~Window() { Close(); }

void Window::Close() {
    if (m_handle) {
        glfwDestroyWindow(m_handle);
        m_handle = nullptr;
    }
}

} // namespace framework
