#pragma once

#include "renderer.h"
#include "scene.h"

namespace framework {

class RenderSystem {
public:
    RenderSystem();
    ~RenderSystem() = default;

    void Render(Scene &scene);

private:
    MeshRenderer m_mesh_renderer;
};

} // namespace framework
