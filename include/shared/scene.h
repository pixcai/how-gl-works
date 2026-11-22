#pragma once

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

enum class ObjectFlags {
    None,
    Added,
    Removed,
};

struct Object : public std::enable_shared_from_this<Object> {
    struct Stats {
        std::size_t next_id;
        std::vector<std::size_t> reusable_ids;
    };

    inline static constexpr std::size_t INVALID_ID = 0;
    inline static Stats stats{1};

    std::size_t id;
    std::string name;
    ObjectFlags flags = ObjectFlags::None;
    std::shared_ptr<Object> parent;
    std::unordered_map<std::size_t, std::shared_ptr<Object>> children;
    std::unordered_map<std::type_index, std::any> components;

    Object();
    Object(const Object &other)            = delete;
    Object &operator=(const Object &other) = delete;
    Object(Object &&other) noexcept;
    Object &operator=(Object &&other) noexcept;
    ~Object();

    void Attach(std::shared_ptr<Object> parent);
    void Detach();

    template <typename T, typename... Args>
    T &Add(Args &&...args) {
        components.try_emplace(typeid(T), T(std::forward<Args>(args)...));
        return Get<T>();
    }

    template <typename T>
    void Remove() {
        components.erase(typeid(T));
    }

    template <typename T>
    bool Has() const {
        return components.contains(typeid(T));
    }

    template <typename T>
    T &Get() {
        return std::any_cast<T &>(components.at(typeid(T)));
    }

    template <typename T>
    const T &Get() const {
        return std::any_cast<T &>(components.at(typeid(T)));
    }
};

struct Scene {
    std::shared_ptr<Object> root;

    Scene();
    ~Scene() = default;

    std::shared_ptr<Object> Get(std::size_t id);
    void Add(std::shared_ptr<Object> object);
    void Remove(std::size_t id);

    void Traverse(std::shared_ptr<Object> &object, const std::function<void(std::shared_ptr<Object> &child)> &callback);

    void Traverse(const std::shared_ptr<Object> &object,
                  const std::function<void(const std::shared_ptr<Object> &child)> &callback) const;

private:
    std::unordered_map<std::size_t, std::shared_ptr<Object>> m_objects;
};
