#include "mesh.h"
#include "scene.h"

#include <utility>

#include <spdlog/spdlog.h>

Object::Object() {
    if (stats.reusable_ids.empty()) {
        id = stats.next_id++;
    } else {
        id = stats.reusable_ids.back();
        stats.reusable_ids.pop_back();
    }
}

Object::Object(Object &&other) noexcept {
    id         = std::exchange(other.id, INVALID_ID);
    name       = std::move(other.name);
    parent     = std::move(other.parent);
    children   = std::move(other.children);
    components = std::move(other.components);
}

Object &Object::operator=(Object &&other) noexcept {
    if (&other == this) {
        return *this;
    }
    id         = std::exchange(other.id, INVALID_ID);
    name       = std::move(other.name);
    parent     = std::move(other.parent);
    children   = std::move(other.children);
    components = std::move(other.components);
    return *this;
}

Object::~Object() {
    if (id != INVALID_ID) {
        stats.reusable_ids.push_back(id);
    }
}

void Object::Attach(std::shared_ptr<Object> parent) {
    if (id == INVALID_ID) {
        spdlog::warn("Cannot attach object: invalid object id");
        return;
    }
    if (this->parent) {
        if (this->parent == parent) {
            return;
        }
        Detach();
    }
    parent->children.try_emplace(id, shared_from_this());
    this->parent = parent;
}

void Object::Detach() {
    if (id == INVALID_ID) {
        spdlog::warn("Cannot detach object: invalid object id");
        return;
    }
    if (parent) {
        parent->children.erase(id);
    }
    parent = nullptr;
}

Scene::Scene()
    : root(std::make_shared<Object>()) {}

std::shared_ptr<Object> Scene::Get(std::size_t id) { return m_objects[id]; }

void Scene::Add(std::shared_ptr<Object> object) {
    if (object->id == Object::INVALID_ID) {
        spdlog::warn("Cannot add object: invalid object id");
        return;
    }
    object->Attach(root);
    object->flags = ObjectFlags::Added;
    m_objects.try_emplace(object->id, object);
    Traverse(object, [&](auto child) {
        child->flags = ObjectFlags::Added;
        m_objects.try_emplace(child->id, std::move(child));
    });
}

void Scene::Remove(std::size_t id) {
    if (auto object = m_objects[id]) {
        object->Detach();
        object->flags = ObjectFlags::Removed;
        Traverse(object, [&](auto child) {
            m_objects.erase(child->id);
            child->flags = ObjectFlags::Removed;
        });
    }
}

void Scene::Traverse(std::shared_ptr<Object> &object,
                     const std::function<void(std::shared_ptr<Object> &child)> &callback) {
    for (auto &[_, child] : object->children) {
        callback(child);
    }
    for (auto &[_, child] : object->children) {
        Traverse(child, callback);
    }
}

void Scene::Traverse(const std::shared_ptr<Object> &object,
                     const std::function<void(const std::shared_ptr<Object> &child)> &callback) const {
    for (const auto &[_, child] : object->children) {
        callback(child);
    }
    for (const auto &[_, child] : object->children) {
        Traverse(child, callback);
    }
}
