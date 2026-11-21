#pragma once

#include "camera_controller.h"
#include "event_manager.h"
#include "gpu_buffer.h"
#include "resource_manager.h"
#include "scene.h"
#include "shader_manager.h"

#include <memory>

#include <GLFW/glfw3.h>

struct Context {
    Scene scene;
    std::unique_ptr<GPUDrawBuffer> draw_buffer;
    CameraController camera_controller;
    EventManager *event_manager{nullptr};
    ShaderManager *shader_manager{nullptr};
    ResourceManager resource_manager;
};

struct MainLoop {
    std::unique_ptr<Context> M;

    virtual void Init() {}
    virtual void Render() {}
};

class App {
public:
    App(const char *title);
    App(const char *title, int width, int height);
    ~App();

    int Run(MainLoop &&main_loop);

private:
    GLFWwindow *m_window;
    EventManager m_event_manager{};
    ShaderManager m_shader_manager{};
};
