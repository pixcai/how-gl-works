#include "framework/framework.h"

#include <glad/gl.h>

struct Sample : public framework::App {
    void Render() override {
        glClearColor(0, 0, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
};

int main() {
    framework::Run<Sample>("Skeleton");
    return 0;
}
