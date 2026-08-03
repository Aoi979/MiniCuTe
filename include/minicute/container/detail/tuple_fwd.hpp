#pragma once

#include <minicute/detail/utility.hpp>

#include <cstddef>

namespace minicute {

template <class... Ts> class tuple;

template <class T> struct tuple_size;

template <class T> inline constexpr std::size_t tuple_size_v = tuple_size<T>::value;

template <std::size_t I, class T> struct tuple_element;

template <std::size_t I, class T> using tuple_element_t = typename tuple_element<I, T>::type;

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
