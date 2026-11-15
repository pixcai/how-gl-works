#pragma once

#include "framework/ecs.h"

#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

namespace framework {

class Scene {
public:
    struct Node : public std::enable_shared_from_this<Node> {
        Scene *scene;
        ecs::Entity entity;
        std::shared_ptr<Node> parent;
        std::unordered_map<ecs::Entity, std::shared_ptr<Node>> children;

        bool visible{true};

        Node(Scene *scene, ecs::Entity entity, std::shared_ptr<Node> parent)
            : scene(scene), entity(entity), parent(parent) {}

        void Destroy() { scene->RemoveNode(shared_from_this()); }

        template <typename ComponentT>
        ComponentT *GetComponent() {
            return scene->m_world.GetComponent<ComponentT>(entity);
        }

        template <typename ComponentT, typename... Args>
        ComponentT *AddComponent(Args &&...args) {
            return scene->m_world.AddComponent<ComponentT>(entity, std::forward<Args>(args)...);
        }

        template <typename ComponentT>
        void RemoveComponent() {
            scene->m_world.RemoveComponent<ComponentT>(entity);
        }
    };

    std::shared_ptr<Node> AddNode(std::shared_ptr<Node> parent = nullptr);
    void RemoveNode(std::shared_ptr<Node> node);

public:
    glm::vec4 background_color{0.0f, 0.0f, 0.0f, 1.0f};

private:
    ecs::World m_world;
    std::unordered_map<ecs::Entity, std::shared_ptr<Node>> m_nodes;
};

} // namespace framework
