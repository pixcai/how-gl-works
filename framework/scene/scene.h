#pragma once

#include "framework/core/ecs.h"
#include "framework/rendering/renderer/render_item.h"

#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

namespace framework {

class Scene {
public:
    struct Node : public std::enable_shared_from_this<Node> {
        Scene &scene;
        ecs::Entity entity;
        std::shared_ptr<Node> parent;
        std::unordered_map<ecs::Entity, std::shared_ptr<Node>> children;

        bool visible{true};

        Node(Scene &scene, ecs::Entity entity) : scene(scene), entity(entity) {}

        void Destroy() { scene.RemoveNode(shared_from_this()); }

        void Attach(std::shared_ptr<Node> child) {
            if (auto [_, success] = children.try_emplace(child->entity, child); success) {
                child->parent = shared_from_this();
            }
        }

        void Detach(std::shared_ptr<Node> child) {
            children.erase(child->entity);
            scene.root->Attach(child);
        }

        template <typename ComponentT>
        ComponentT *GetComponent() {
            return scene.m_world.GetComponent<ComponentT>(entity);
        }

        template <typename ComponentT, typename... Args>
        ComponentT *AddComponent(Args &&...args) {
            return scene.m_world.AddComponent<ComponentT>(entity, std::forward<Args>(args)...);
        }

        template <typename ComponentT>
        void RemoveComponent() {
            scene.m_world.RemoveComponent<ComponentT>(entity);
        }
    };

public:
    Scene();
    ~Scene();

    std::shared_ptr<Node> AddNode(std::shared_ptr<Node> parent = nullptr);
    void RemoveNode(std::shared_ptr<Node> node);

    void GetEntities(std::shared_ptr<Node> node, std::vector<ecs::Entity> &entities);
    void CollectRenderItems(std::shared_ptr<Node> node, std::vector<RenderItem> &items);

public:
    glm::vec4 background_color{0.0f, 0.0f, 0.0f, 1.0f};
    std::shared_ptr<Node> root;

private:
    ecs::World m_world;
};

} // namespace framework
