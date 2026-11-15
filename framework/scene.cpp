#include "scene.h"

namespace framework {

std::shared_ptr<Scene::Node> Scene::AddNode(std::shared_ptr<Node> parent) {
    auto entity = m_world.CreateEntity();

    m_nodes.emplace(entity, std::make_shared<Node>(this, entity, parent));
    return m_nodes[entity];
}

void Scene::RemoveNode(std::shared_ptr<Node> node) {
    m_nodes.erase(node->entity);
    m_world.DestroyEntity(node->entity);

    for (auto &[_, child] : node->children) {
        RemoveNode(child);
    }
}

} // namespace framework
