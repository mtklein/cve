#pragma once

#include <type_traits>

template <class T>
inline bool equiv(std::type_identity_t<T> x, T y) {
    if constexpr (std::is_floating_point_v<T>) {
        return (x <= y && y <= x) || (x != x && y != y);
    } else {
        return x == y;
    }
}
