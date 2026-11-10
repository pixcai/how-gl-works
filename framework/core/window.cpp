#include "framework.h"
#include "utils/logger.h"

#include <stdexcept>

#include <GLFW/glfw3.h>

namespace Framework {

Window::Window(int width, int height, const char *title) {
    if (m_handle = glfwCreateWindow(width, height, title, nullptr, nullptr); !m_handle) {
        throw std::runtime_error("Error: Failed to create window");
    }
    glfwGetFramebufferSize(m_handle, &m_framebuffer_size.x, &m_framebuffer_size.y);
    logInfo("Window size: ({}, {}), Framebuffer size: ({}, {})", width, height, m_framebuffer_size.x,
            m_framebuffer_size.y);
}

Window::~Window() {
    if (m_handle) {
        glfwDestroyWindow(m_handle);
        m_handle = nullptr;
    }
}

} // namespace Framework
