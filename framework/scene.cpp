#include "ecs.h"
#include "render_item.h"
#include "scene.h"

#include <memory>
#include <vector>

namespace framework {

Scene::Scene() { root = std::make_shared<Node>(*this, m_world.CreateEntity()); }

Scene::~Scene() { RemoveNode(root); }

std::shared_ptr<Scene::Node> Scene::AddNode(std::shared_ptr<Node> parent) {
    auto node = std::make_shared<Node>(*this, m_world.CreateEntity());

    if (parent) {
        parent->Attach(node);
    } else {
        root->Attach(node);
    }
    return node;
}

void Scene::RemoveNode(std::shared_ptr<Node> node) {
    std::vector<ecs::Entity> entities;

    if (node->parent) {
        node->parent->children.erase(node->entity);
        node->parent = nullptr;
    }
    GetEntities(node, entities);
    for (auto entity : entities) {
        m_world.DestroyEntity(entity);
    }
}

void Scene::GetEntities(std::shared_ptr<Node> node, std::vector<ecs::Entity> &entities) {
    entities.push_back(node->entity);

    for (auto &[_, child] : node->children) {
        GetEntities(child, entities);
    }
}

void Scene::CollectRenderItems(std::shared_ptr<Node> node, std::vector<RenderItem> &items) {
    if (!node->visible) {
        return;
    }

    auto component = node->GetComponent<MeshComponent>();
    if (component) {
        items.emplace_back(&component->mesh);
    }
    for (auto [_, child] : node->children) {
        CollectRenderItems(child, items);
    }
}

} // namespace framework
