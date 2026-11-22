#pragma once

#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>

struct EventState {
    bool connected{true};
};

struct EventConnection {
    EventConnection(const std::shared_ptr<EventState> &state, std::function<void()> disconnect_callback)
        : m_state(state)
        , m_disconnect_callback(std::move(disconnect_callback)) {}

    void Disconnect() const {
        if (auto state = m_state.lock()) {
            if (state->connected) {
                state->connected = false;
                m_disconnect_callback();
            }
        }
    }

private:
    std::weak_ptr<EventState> m_state;
    std::function<void()> m_disconnect_callback;
};

struct IEventListener {
    virtual ~IEventListener() = default;
};

template <typename T>
struct EventListener : public IEventListener {
    std::shared_ptr<EventState> state;
    std::function<void(const T &event)> callback;

    EventListener(std::shared_ptr<EventState> state, std::function<void(const T &event)> callback)
        : state(std::move(state))
        , callback(std::move(callback)) {}
};

class EventManager {
public:
    template <typename T>
    EventConnection Subscribe(std::function<void(const T &event)> callback) {
        auto state    = std::make_shared<EventState>();
        auto listener = std::make_shared<EventListener<T>>(state, std::move(callback));

        m_listeners.emplace(typeid(T), listener);
        return {state, [this, listener] { RemoveListener<T>(listener); }};
    }

    template <typename T>
    void Dispatch(const T &event) {
        auto [first, last] = m_listeners.equal_range(typeid(T));

        for (auto it = first; it != last; ++it) {
            auto listener = static_cast<EventListener<T> *>(it->second.get());

            if (listener->state->connected) {
                listener->callback(event);
            }
        }
    }

private:
    template <typename T>
    void RemoveListener(const std::shared_ptr<IEventListener> &listener) {
        auto [first, last] = m_listeners.equal_range(typeid(T));

        for (auto it = first; it != last; ++it) {
            if (it->second == listener) {
                m_listeners.erase(it);
                break;
            }
        }
    }

    std::unordered_multimap<std::type_index, std::shared_ptr<IEventListener>> m_listeners;
};
