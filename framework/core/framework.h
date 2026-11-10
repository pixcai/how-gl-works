#pragma once

#include <memory>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Framework {

class Window {
public:
    Window(int width, int height, const char *title);
    ~Window();

    GLFWwindow *handle() const { return m_handle; }
    glm::ivec2 framebufferSize() const { return m_framebuffer_size; }

private:
    GLFWwindow *m_handle;
    glm::ivec2 m_framebuffer_size;
};

struct ApplicationOptions {
    int width;
    int height;
    int samples;
    int glsl_major;
    int glsl_minor;
};

class Application {
public:
    Application(const char *title);
    Application(const char *title, const ApplicationOptions &options);
    virtual ~Application();

    void run();

    virtual void render() = 0;
    virtual void onFramebufferSizeCallback(int width, int height);
    virtual void onMouseButtonCallback(int button, int action, int mods);
    virtual void onCursorPosCallback(double x, double y);
    virtual void onScrollCallback(double dx, double dy);
    virtual void onKeyCallback(int key, int scancode, int action, int mods);

public:
    ApplicationOptions options;
    std::unique_ptr<Window> window;
};

}; // namespace Framework
