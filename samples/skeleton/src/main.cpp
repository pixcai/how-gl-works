#include "framework/framework.h"
#include "framework/shader.h"

#include <glad/gl.h>

using namespace framework;

struct Sample : public App {
    void Init(Context *context) override {
        App::Init(context);
        Shader shader{"vs_main.glsl", "fs_main.glsl"};
    }

    void Render() override {
        glClearColor(0, 0, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
};

int main() {
    Run<Sample>("Skeleton");
    return 0;
}
