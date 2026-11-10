#include "core/framework.h"

struct Sample : public Framework::Application {
    using Framework::Application::Application;

    void render() override {}
};

int main() {
    Sample("Skeleton").run();
    return 0;
}
