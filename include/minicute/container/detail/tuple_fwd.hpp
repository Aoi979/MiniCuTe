#pragma once

#include <minicute/util/type_traits.hpp>

#include <cstddef>

namespace minicute {

template <class... Ts> class tuple;

template <std::size_t I, class... Ts> constexpr decltype(auto) get(tuple<Ts...>& t) noexcept;

template <std::size_t I, class... Ts> constexpr decltype(auto) get(const tuple<Ts...>& t) noexcept;

template <std::size_t I, class... Ts> constexpr decltype(auto) get(tuple<Ts...>&& t) noexcept;

template <std::size_t I, class... Ts> constexpr decltype(auto) get(const tuple<Ts...>&& t) noexcept;

template <class T, class... Ts> constexpr decltype(auto) get(tuple<Ts...>& t) noexcept;

template <class T, class... Ts> constexpr decltype(auto) get(const tuple<Ts...>& t) noexcept;

template <class T, class... Ts> constexpr decltype(auto) get(tuple<Ts...>&& t) noexcept;

template <class T, class... Ts> constexpr decltype(auto) get(const tuple<Ts...>&& t) noexcept;

} // namespace minicute

namespace minicute::detail {

} // namespace minicute::detail
