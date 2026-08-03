#pragma once

#include <minicute/container/detail/tuple_fwd.hpp>
#include <minicute/container/detail/tuple_storage.hpp>
#include <minicute/container/detail/tuple_traits.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace minicute::detail {

template <class T, class... Ts>
inline constexpr std::size_t type_count_v =
    (std::size_t{0} + ... + (std::is_same_v<T, Ts> ? std::size_t{1} : std::size_t{0}));

template <class T, std::size_t I, class... Ts> struct type_index_impl {
    static_assert(always_false_v<T>, "minicute::get<T> type is not present");
};

template <class T, std::size_t I, class First, class... Rest>
struct type_index_impl<T, I, First, Rest...> : type_index_impl<T, I + 1, Rest...> {};

template <class T, std::size_t I, class... Rest>
struct type_index_impl<T, I, T, Rest...> : std::integral_constant<std::size_t, I> {};

template <class T, class... Ts>
inline constexpr std::size_t type_index_v = type_index_impl<T, 0, Ts...>::value;

} // namespace minicute::detail

namespace minicute {

template <std::size_t I, class... Ts> constexpr decltype(auto) get(tuple<Ts...>& t) noexcept {
    return static_cast<detail::tuple_leaf<I, detail::type_at_t<I, Ts...>>&>(t).get();
}

template <std::size_t I, class... Ts> constexpr decltype(auto) get(const tuple<Ts...>& t) noexcept {
    return static_cast<const detail::tuple_leaf<I, detail::type_at_t<I, Ts...>>&>(t).get();
}

template <std::size_t I, class... Ts> constexpr decltype(auto) get(tuple<Ts...>&& t) noexcept {
    return static_cast<detail::tuple_leaf<I, detail::type_at_t<I, Ts...>>&&>(t).get();
}

template <std::size_t I, class... Ts>
constexpr decltype(auto) get(const tuple<Ts...>&& t) noexcept {
    return static_cast<const detail::tuple_leaf<I, detail::type_at_t<I, Ts...>>&&>(t).get();
}

template <class T, class... Ts> constexpr decltype(auto) get(tuple<Ts...>& t) noexcept {
    static_assert(detail::type_count_v<T, Ts...> == 1,
                  "minicute::get<T> requires T to appear exactly once");

    if constexpr (detail::type_count_v<T, Ts...> == 1) {
        return get<detail::type_index_v<T, Ts...>>(t);
    }
}

template <class T, class... Ts> constexpr decltype(auto) get(const tuple<Ts...>& t) noexcept {
    static_assert(detail::type_count_v<T, Ts...> == 1,
                  "minicute::get<T> requires T to appear exactly once");

    if constexpr (detail::type_count_v<T, Ts...> == 1) {
        return get<detail::type_index_v<T, Ts...>>(t);
    }
}

template <class T, class... Ts> constexpr decltype(auto) get(tuple<Ts...>&& t) noexcept {
    static_assert(detail::type_count_v<T, Ts...> == 1,
                  "minicute::get<T> requires T to appear exactly once");

    if constexpr (detail::type_count_v<T, Ts...> == 1) {
        return get<detail::type_index_v<T, Ts...>>(std::move(t));
    }
}

template <class T, class... Ts> constexpr decltype(auto) get(const tuple<Ts...>&& t) noexcept {
    static_assert(detail::type_count_v<T, Ts...> == 1,
                  "minicute::get<T> requires T to appear exactly once");

    if constexpr (detail::type_count_v<T, Ts...> == 1) {
        return get<detail::type_index_v<T, Ts...>>(std::move(t));
    }
}

} // namespace minicute
