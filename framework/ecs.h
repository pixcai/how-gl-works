#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace framework::ecs {

using Entity = std::uint32_t;

struct BaseStorage {
    virtual ~BaseStorage()             = default;
    virtual void Remove(Entity entity) = 0;
};

template <typename ComponentT>
class Storage : public BaseStorage {
public:
    ComponentT *Get(Entity entity) {
        if (!m_entity_indices.contains(entity)) {
            return nullptr;
        }
        return &m_components[m_entity_indices[entity]];
    }

    template <typename... Args>
    ComponentT *Add(Entity entity, Args &&...args) {
        m_components.emplace_back(std::forward<Args>(args)...);
        m_entity_indices[entity] = m_components.size() - 1;
        return &m_components.back();
    }

    void Remove(Entity entity) override {
        if (!m_entity_indices.contains(entity)) {
            return;
        }

        auto index      = m_entity_indices[entity];
        auto last_index = m_components.size() - 1;

        std::swap(m_components[index], m_components[last_index]);
        m_components.pop_back();
        m_entity_indices.erase(entity);
        for (auto [key, value] : m_entity_indices) {
            if (value == last_index) {
                m_entity_indices[key] = index;
                break;
            }
        }
    }

private:
    std::unordered_map<Entity, std::size_t> m_entity_indices;
    std::vector<ComponentT> m_components;
};

class World {
public:
    Entity CreateEntity();
    void DestroyEntity(Entity entity);

    template <typename ComponentT>
    ComponentT *GetComponent(Entity entity) {
        auto index = std::type_index(typeid(ComponentT));

        if (!m_storages.contains(index)) {
            return nullptr;
        }

        auto storage = static_cast<Storage<ComponentT> *>(m_storages[index].get());

        return storage->Get(entity);
    }

    template <typename ComponentT, typename... Args>
    ComponentT *AddComponent(Entity entity, Args &&...args) {
        auto index = std::type_index(typeid(ComponentT));

        if (!m_storages.contains(index)) {
            m_storages[index] = std::make_unique<Storage<ComponentT>>();
        }

        auto storage = static_cast<Storage<ComponentT> *>(m_storages[index].get());

        return storage->Add(entity, std::forward<Args>(args)...);
    }

    template <typename ComponentT>
    void RemoveComponent(Entity entity) {
        auto index = std::type_index(typeid(ComponentT));

        if (!m_storages.contains(index)) {
            return;
        }

        auto storage = static_cast<Storage<ComponentT> *>(m_storages[index].get());

        return storage->Remove(entity);
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<BaseStorage>> m_storages;
};

} // namespace framework::ecs
