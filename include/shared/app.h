#pragma once

#include "camera_controller.h"
#include "event_manager.h"
#include "scene.h"
#include "shader_manager.h"

#include <memory>

#include <GLFW/glfw3.h>

struct Context {
    Scene scene;
    CameraController camera_controller;
    EventManager *event_manager;
    ShaderManager *shader_manager;
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
