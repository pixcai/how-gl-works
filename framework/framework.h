#pragma once

#include <concepts>
#include <memory>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace framework {

struct Context {
    glm::ivec2 framebuffer_size;
};

class App {
public:
    App() = default;
    virtual ~App() = default;

    Context *GetContext() { return m_context; }

    virtual void Init(Context *context);
    virtual void Render() = 0;

    virtual void OnFramebufferSizeCallback(int width, int height);
    virtual void OnKeyCallback(int key, int scancode, int action, int mods);
    virtual void OnMouseButtonCallback(int button, int action, int mods);
    virtual void OnCursorCallback(double x, double y);
    virtual void OnScrollCallback(double dx, double dy);

private:
    Context *m_context;
};

class Runner {
public:
    Runner(std::unique_ptr<App> app, const char *title);
    ~Runner();

    void Run();

private:
    void Init(const char *title, int width, int height);

private:
    Context m_context;
    GLFWwindow *m_window;
    std::unique_ptr<App> m_app;
};

template <typename T, typename... Args>
    requires std::derived_from<T, App>
inline void Run(const char *title, Args &&...args) {
    Runner(std::make_unique<T>(std::forward<Args>(args)...), title).Run();
}

}; // namespace framework
