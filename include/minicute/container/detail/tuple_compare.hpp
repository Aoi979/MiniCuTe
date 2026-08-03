#pragma once

#include <minicute/container/detail/tuple_access.hpp>
#include <minicute/container/detail/tuple_fwd.hpp>
#include <minicute/container/detail/tuple_traits.hpp>

#include <type_traits>
#include <utility>

namespace minicute::detail {

template <class... Ts, std::size_t... Is>
constexpr void swap_impl(tuple<Ts...>& lhs, tuple<Ts...>& rhs,
                         std::index_sequence<Is...>) noexcept((std::is_nothrow_swappable_v<Ts> &&
                                                               ...)) {
    using std::swap;
    (swap(minicute::get<Is>(lhs), minicute::get<Is>(rhs)), ...);
}

template <class Lhs, class Rhs, std::size_t... Is>
constexpr bool equal_impl(const Lhs& lhs, const Rhs& rhs, std::index_sequence<Is...>) noexcept(
    noexcept(((minicute::get<Is>(lhs) == minicute::get<Is>(rhs)) && ...))) {
    return ((minicute::get<Is>(lhs) == minicute::get<Is>(rhs)) && ...);
}

template <std::size_t I, class Lhs, class Rhs>
constexpr bool less_impl(const Lhs& lhs, const Rhs& rhs) {
    if constexpr (I == tuple_size_v<Lhs>) {
        return false;
    } else {
        if (!(minicute::get<I>(lhs) == minicute::get<I>(rhs))) {
            return minicute::get<I>(lhs) < minicute::get<I>(rhs);
        }

        return less_impl<I + 1>(lhs, rhs);
    }
}

} // namespace minicute::detail

namespace minicute {

template <class... Ts>
constexpr void swap(tuple<Ts...>& lhs,
                    tuple<Ts...>& rhs) noexcept((std::is_nothrow_swappable_v<Ts> && ...))
    requires(std::is_swappable_v<Ts> && ...)
{
    detail::swap_impl(lhs, rhs, std::index_sequence_for<Ts...>{});
}

template <class... Ts, class... Us>
    requires(sizeof...(Ts) == sizeof...(Us))
constexpr bool operator==(const tuple<Ts...>& lhs, const tuple<Us...>& rhs) noexcept(
    noexcept(detail::equal_impl(lhs, rhs, std::make_index_sequence<sizeof...(Ts)>{}))) {
    return detail::equal_impl(lhs, rhs, std::make_index_sequence<sizeof...(Ts)>{});
}

template <class... Ts, class... Us>
constexpr bool operator!=(const tuple<Ts...>& lhs, const tuple<Us...>& rhs) {
    return !(lhs == rhs);
}

template <class... Ts, class... Us>
    requires(sizeof...(Ts) == sizeof...(Us))
constexpr bool operator<(const tuple<Ts...>& lhs, const tuple<Us...>& rhs) {
    return detail::less_impl<0>(lhs, rhs);
}

template <class... Ts, class... Us>
constexpr bool operator>(const tuple<Ts...>& lhs, const tuple<Us...>& rhs) {
    return rhs < lhs;
}

template <class... Ts, class... Us>
constexpr bool operator<=(const tuple<Ts...>& lhs, const tuple<Us...>& rhs) {
    return !(rhs < lhs);
}

template <class... Ts, class... Us>
constexpr bool operator>=(const tuple<Ts...>& lhs, const tuple<Us...>& rhs) {
    return !(lhs < rhs);
}

} // namespace minicute
