#include "render_item.h"
#include "render_system.h"

#include <vector>

namespace framework {

RenderSystem::RenderSystem() : m_mesh_renderer() {}

void RenderSystem::Render(Scene &scene) {
    std::vector<RenderItem> items;

    scene.CollectRenderItems(scene.root, items);
    for (const auto &item : items) {
        m_mesh_renderer.Render(item);
    }
}

} // namespace framework
