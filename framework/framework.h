#pragma once

#include "framework/context.h"
#include "framework/window.h"

#include <memory>
#include <string>

namespace framework {

class App {
public:
    App(std::string title = "Sample");
    virtual ~App() = default;

    int Run();

    virtual void Init();
    virtual void Render() = 0;

    virtual void OnFramebufferResize(int width, int height);
    virtual void OnKeyPressed(int key, int scancode, int action, int mods);
    virtual void OnMousePressed(int button, int action, int mods);
    virtual void OnMouseMove(double dx, double dy);
    virtual void OnMouseWheel(double dx, double dy);

private:
    void MainLoop();

protected:
    std::string title;
    Context context;
    std::unique_ptr<Window> window;
};

}; // namespace framework
