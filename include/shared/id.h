#pragma once

#include <cstddef>
#include <functional>

template <typename T>
struct ID {
    std::size_t value{++next_value};

    bool operator==(const ID &id) const = default;

    void Reset() noexcept { value = 0; }
    bool IsValid() const noexcept { return value != 0; }

private:
    inline static std::size_t next_value;
};

namespace std {

template <typename T>
struct hash<ID<T>> {
    std::size_t operator()(const ID<T> &id) const noexcept { return hash<std::size_t>{}(id.value); }
};

} // namespace std
