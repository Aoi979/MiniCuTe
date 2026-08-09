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

namespace detail {

template <class FlatShape, class FlatOrder, std::size_t... Is>
constexpr bool static_compact_order_leaves(
    std::index_sequence<Is...>) {
  return ((is_static_integral_v<decltype(get<Is>(
                    std::declval<FlatShape const &>()))> &&
           is_static_integral_v<decltype(get<Is>(
                    std::declval<FlatOrder const &>()))>) &&
          ...);
}

template <class FlatShape, class FlatOrder, std::size_t... Is>
constexpr auto compact_order_impl(FlatShape const &shape, FlatOrder const &order,
                                  std::index_sequence<Is...>) {
  auto stride_for = [&](auto const &current_order) {
    // A mode with order p starts after all modes whose order is less than p.
    return product(transform(
        shape, order, [&](auto const &mode_shape, auto const &mode_order) {
          return conditional_return(mode_order < current_order, mode_shape,
                                    _1{});
        }));
  };

  return make_stride(stride_for(get<Is>(order))...);
}

} // namespace detail

template <class Shape, class Order>
constexpr auto compact_order(Shape const &shape, Order const &order) {
  auto flat_shape = flatten(shape);
  auto flat_order = flatten(order);

  static_assert(tuple_size_v<decltype(flat_shape)> ==
                    tuple_size_v<decltype(flat_order)>,
                "compact_order requires matching shape and order profiles");

  static_assert(detail::static_compact_order_leaves<
                    decltype(flat_shape), decltype(flat_order)>(
                    std::make_index_sequence<tuple_size_v<
                        decltype(flat_shape)>>{}),
                "compact_order requires static Shape and Order");

  auto flat_stride = detail::compact_order_impl(
      flat_shape, flat_order,
      std::make_index_sequence<tuple_size_v<decltype(flat_shape)>>{});

  return unflatten(flat_stride, shape);
}

} // namespace minicute
