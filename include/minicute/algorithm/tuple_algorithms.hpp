#pragma once

#include <minicute/container/tuple.hpp>

#include <cstddef>
#include <utility>

namespace minicute {

namespace detail {

template <class Tuple, class F, std::size_t... Is>
constexpr auto for_each_impl(Tuple const& tuple, F&& f,
                             std::index_sequence<Is...>) {
  static_assert(is_tuple_v<Tuple>,
                "for_each_impl only works on tuples");
  (f(get<Is>(tuple)), ...);
}

template <class Tuple, class F, std::size_t... Is>
constexpr auto apply_impl(Tuple const& tuple, F&& f,
                          std::index_sequence<Is...>) {
  static_assert(is_tuple_v<Tuple>, "apply_impl only works on tuples");
  return f(get<Is>(tuple)...);
}

template <class Tuple, class F, std::size_t... Is>
constexpr auto transform_impl(Tuple const& values, F&& f,
                              std::index_sequence<Is...>) {
  static_assert(is_tuple_v<Tuple>,
                "transform_impl only works on tuples");
  return minicute::tuple<remove_cvref_t<decltype(f(get<Is>(values)))>...>{
      f(get<Is>(values))...};
}

template <class TupleA, class TupleB, class F, std::size_t... Is>
constexpr auto transform_impl(TupleA const& a, TupleB const& b, F&& f,
                              std::index_sequence<Is...>) {
  static_assert(is_tuple_v<TupleA> && is_tuple_v<TupleB>,
                "transform_impl only works on tuples");
  static_assert(tuple_size_v<TupleA> == tuple_size_v<TupleB>,
                "transform_impl requires equal sizes");
  return tuple<remove_cvref_t<decltype(f(get<Is>(a), get<Is>(b)))>...>{
      f(get<Is>(a), get<Is>(b))...};
}

}  // namespace detail

template <class Tuple, class F>
constexpr auto for_each(Tuple const& tuple, F&& f) {
  static_assert(is_tuple_v<Tuple>, "for_each only works on tuples");
  return detail::for_each_impl(
      tuple, std::forward<F>(f),
      std::make_index_sequence<tuple_size_v<Tuple>>{});
}

template <class Tuple, class F>
constexpr auto apply(Tuple const& tuple, F&& f) {
  static_assert(is_tuple_v<Tuple>, "apply only works on tuples");
  return detail::apply_impl(
      tuple, std::forward<F>(f),
      std::make_index_sequence<tuple_size_v<Tuple>>{});
}

template <class Tuple, class F>
constexpr auto transform(Tuple const& tuple, F&& f) {
  static_assert(is_tuple_v<Tuple>, "transform only works on tuples");
  return detail::transform_impl(
      tuple, std::forward<F>(f),
      std::make_index_sequence<tuple_size_v<Tuple>>{});
}

template <class TupleA, class TupleB, class F>
constexpr auto transform(TupleA const& a, TupleB const& b, F&& f) {
  static_assert(is_tuple_v<TupleA> && is_tuple_v<TupleB>,
                "transform only works on tuples");
  static_assert(tuple_size_v<TupleA> == tuple_size_v<TupleB>,
                "transform requires equal sizes");
  return detail::transform_impl(
      a, b, std::forward<F>(f),
      std::make_index_sequence<tuple_size_v<TupleA>>{});
}

}  // namespace minicute
