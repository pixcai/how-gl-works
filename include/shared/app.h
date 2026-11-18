#pragma once

#include "event_manager.h"

#include <GLFW/glfw3.h>

struct MainLoop {
    EventManager event_manager;

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
};
