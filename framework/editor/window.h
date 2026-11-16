#pragma once

#include "framework/core/context.h"

#include <functional>

#include <glm/glm.hpp>
#define GLFW_INCLUDE_NONE 1
#include <GLFW/glfw3.h>

namespace framework {

class Window {
public:
    static void DrawMainMenuBar(Context &context);
    static void DrawStatusBar(Context &context);

    static void OnFramebufferResize(GLFWwindow *handle, int width, int height);
    static void OnKeyPressed(GLFWwindow *handle, int key, int scancode, int action, int mods);
    static void OnMousePressed(GLFWwindow *handle, int button, int action, int mods);
    static void OnMouseMove(GLFWwindow *handle, double dx, double dy);
    static void OnMouseWheel(GLFWwindow *handle, double dx, double dy);

public:
    Window(const char *title, int width, int height);
    ~Window();

    void Close();
    GLFWwindow *GetHandle() const { return m_handle; }
    glm::ivec2 GetFramebufferSize() const { return m_framebuffer_size; }

public:
    std::function<void(int width, int height)> on_framebuffer_resize;
    std::function<void(int key, int scancode, int action, int mods)> on_key_pressed;
    std::function<void(int button, int action, int mods)> on_mouse_pressed;
    std::function<void(double dx, double dy)> on_mouse_move;
    std::function<void(double dx, double dy)> on_mouse_wheel;

private:
    GLFWwindow *m_handle;
    glm::ivec2 m_framebuffer_size;
};

} // namespace framework
