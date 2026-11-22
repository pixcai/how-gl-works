#pragma once

#include "id.h"

#include <memory>
#include <typeindex>
#include <unordered_map>

struct IResourceStore {
    virtual ~IResourceStore() = default;
};

template <typename T>
struct ResourceStore : public IResourceStore {
    std::unordered_map<ID<T>, T> entries;
};

struct ResourceManager {
    template <typename T, typename... Args>
    ID<T> Create(Args &&...args) {
        if (!m_stores.contains(typeid(T))) {
            m_stores.try_emplace(typeid(T), std::make_unique<ResourceStore<T>>());
        }

        ID<T> id;
        GetStore<T>()->entries.insert_or_assign(id, T(std::forward<Args>(args)...));

        return id;
    }

    template <typename T>
    T *Acquire(ID<T> id) {
        if (auto store = GetStore<T>()) {
            if (auto it = store->entries.find(id); it != store->entries.end()) {
                return &it->second;
            }
        }
        return nullptr;
    }

    template <typename T>
    void Release(ID<T> id) {
        if (auto store = GetStore<T>()) {
            if (auto it = store->entries.find(id); it != store->entries.end()) {
                store->entries.erase(it);
            }
        }
    }

private:
    template <typename T>
    ResourceStore<T> *GetStore() {
        if (auto it = m_stores.find(typeid(T)); it != m_stores.end()) {
            return static_cast<ResourceStore<T> *>(it->second.get());
        }
        return nullptr;
    }

    std::unordered_map<std::type_index, std::unique_ptr<IResourceStore>> m_stores;
};
