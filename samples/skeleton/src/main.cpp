#include "framework/framework.h"

struct Sample : public framework::App {
    void Init() override {
        framework::App::Init();

        auto node = scene.AddNode();
        node->AddComponent<framework::MeshComponent>();
    }

    void Render() override { context.render_system.Render(scene); }
};

int main() { return Sample().Run(); }
