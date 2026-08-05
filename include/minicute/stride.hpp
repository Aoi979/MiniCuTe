#pragma once

#include <cstddef>
#include <minicute/int_tuple.hpp>
#include <utility>

namespace minicute {

struct LayoutLeft {};
struct LayoutRight {};

template <class Shape, class Current = _1>
constexpr auto compact_col_major(Shape const &shape,
                                 Current const &current = {});

namespace detail {

template <std::size_t I, class Shape, std::size_t... Is>
constexpr auto col_major_prefix(Shape const &shape,
                                std::index_sequence<Is...>) {
  return (product(get<Is>(shape)) * ... * _1{});
}

template <std::size_t I, class Shape>
constexpr auto col_major_prefix(Shape const &shape) {
  return col_major_prefix<I>(shape, std::make_index_sequence<I>{});
}

template <class Shape, class Current, std::size_t... Is>
constexpr auto compact_col_major_impl(Shape const &shape,
                                      Current const &current,
                                      std::index_sequence<Is...>) {
  return make_stride(compact_col_major(
      get<Is>(shape), current * col_major_prefix<Is>(shape))...);
}

} // namespace detail

template <class Shape, class Current>
constexpr auto compact_col_major(Shape const &shape, Current const &current) {
  if constexpr (is_tuple_v<Shape>) {
    return detail::compact_col_major_impl(
        shape, current, std::make_index_sequence<tuple_size_v<Shape>>{});
  } else {
    return current;
  }
}

template <class Shape, class Current = _1>
constexpr auto compact_row_major(Shape const &shape,
                                 Current const &current = {});

namespace detail {

template <std::size_t I, class Shape, std::size_t... Is>
constexpr auto row_major_suffix(Shape const &shape,
                                std::index_sequence<Is...>) {
  return (product(get<I + 1 + Is>(shape)) * ... * _1{});
}

template <std::size_t I, class Shape>
constexpr auto row_major_suffix(Shape const &shape) {
  return row_major_suffix<I>(
      shape, std::make_index_sequence<tuple_size_v<Shape> - I - 1>{});
}

template <class Shape, class Current, std::size_t... Is>
constexpr auto compact_row_major_impl(Shape const &shape,
                                      Current const &current,
                                      std::index_sequence<Is...>) {
  return make_stride(compact_row_major(
      get<Is>(shape), current * row_major_suffix<Is>(shape))...);
}

} // namespace detail

template <class Shape, class Current>
constexpr auto compact_row_major(Shape const &shape, Current const &current) {
  if constexpr (is_tuple_v<Shape>) {
    return detail::compact_row_major_impl(
        shape, current, std::make_index_sequence<tuple_size_v<Shape>>{});
  } else {
    return current;
  }
}

template <class Shape, class Order>
constexpr auto compact_order(Shape const &, Order const &) {
  static_assert(detail::always_false_v<Shape, Order>,
                "TODO(stage5): implement compact_order");
}

} // namespace minicute
