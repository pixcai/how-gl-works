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
        spdlog::warn("Cannot attach node: invalid node id");
        return;
    }
    if (this->parent) {
        if (this->parent == parent) {
            return;
        }
        Detach();
    }
    this->parent->children.emplace(id, shared_from_this());
    this->parent = parent;
}

void Object::Detach() {
    if (id == INVALID_ID) {
        spdlog::warn("Cannot detach node: invalid node id");
        return;
    }
    if (parent) {
        parent->children.erase(id);
    }
    parent = nullptr;
}

Scene::Scene() : root(std::make_shared<Object>()) {}

void Scene::Traverse(const std::shared_ptr<Object> &object,
                     const std::function<void(std::shared_ptr<Object> &child)> &callback) {
    for (auto &[_, child] : object->children) {
        callback(child);
    }
    for (auto &[_, child] : object->children) {
        Traverse(child, callback);
    }
}
