#pragma once

#include <minicute/container/tuple.hpp>
#include <minicute/numeric/integral_constant.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace minicute {

namespace detail {

template <class Tuple, class F, std::size_t... Is>
constexpr auto for_each_impl(Tuple const &tuple, F &&f,
                             std::index_sequence<Is...>) {
  static_assert(is_tuple_v<Tuple>, "for_each_impl only works on tuples");
  (f(get<Is>(tuple)), ...);
}

template <class Tuple, class F, std::size_t... Is>
constexpr auto apply_impl(Tuple const &tuple, F &&f,
                          std::index_sequence<Is...>) {
  static_assert(is_tuple_v<Tuple>, "apply_impl only works on tuples");
  return f(get<Is>(tuple)...);
}

template <class Tuple, class F, std::size_t... Is>
constexpr auto transform_impl(Tuple const &values, F &&f,
                              std::index_sequence<Is...>) {
  static_assert(is_tuple_v<Tuple>, "transform_impl only works on tuples");
  return minicute::tuple<remove_cvref_t<decltype(f(get<Is>(values)))>...>{
      f(get<Is>(values))...};
}

template <class TupleA, class TupleB, class F, std::size_t... Is>
constexpr auto transform_impl(TupleA const &a, TupleB const &b, F &&f,
                              std::index_sequence<Is...>) {
  static_assert(is_tuple_v<TupleA> && is_tuple_v<TupleB>,
                "transform_impl only works on tuples");
  static_assert(tuple_size_v<TupleA> == tuple_size_v<TupleB>,
                "transform_impl requires equal sizes");
  return tuple<remove_cvref_t<decltype(f(get<Is>(a), get<Is>(b)))>...>{
      f(get<Is>(a), get<Is>(b))...};
}

} // namespace detail

template <class T, class TG>
constexpr auto zip2_by(T const &t, TG const &guide);

namespace detail {

template <class T, class TG, std::size_t... I0, std::size_t... I1>
constexpr auto zip2_by_impl(T const &t, TG const &guide,
                            std::index_sequence<I0...>,
                            std::index_sequence<I1...>) {
  constexpr std::size_t guide_rank = tuple_size_v<TG>;

  auto split = make_tuple(
      minicute::zip2_by(get<I0>(t), get<I0>(guide))...);

  return make_tuple(
      make_tuple(get<0>(get<I0>(split))...),
      make_tuple(get<1>(get<I0>(split))...,
                 get<guide_rank + I1>(t)...));
}

} // namespace detail

template <class T, class TG>
constexpr auto zip2_by(T const &t, TG const &guide) {
  if constexpr (is_tuple_v<T>) {
    if constexpr (is_tuple_v<TG>) {
      constexpr std::size_t value_rank = tuple_size_v<T>;
      constexpr std::size_t guide_rank = tuple_size_v<TG>;

      static_assert(value_rank >= guide_rank,
                    "zip2_by guide rank exceeds value rank");

      return detail::zip2_by_impl(
          t, guide, std::make_index_sequence<guide_rank>{},
          std::make_index_sequence<value_rank - guide_rank>{});
    } else {
      // A scalar guide marks an already formed rank-2 mode.
      static_assert(tuple_size_v<T> == 2,
                    "zip2_by requires a rank-2 terminal");
      return t;
    }
  } else {
    return t;
  }
}

template <class Tuple, class F>
constexpr auto for_each(Tuple const &tuple, F &&f) {
  static_assert(is_tuple_v<Tuple>, "for_each only works on tuples");
  return detail::for_each_impl(tuple, std::forward<F>(f),
                               std::make_index_sequence<tuple_size_v<Tuple>>{});
}

template <class Tuple, class F>
constexpr auto apply(Tuple const &tuple, F &&f) {
  static_assert(is_tuple_v<Tuple>, "apply only works on tuples");
  return detail::apply_impl(tuple, std::forward<F>(f),
                            std::make_index_sequence<tuple_size_v<Tuple>>{});
}

template <class Tuple, class F>
constexpr auto transform(Tuple const &tuple, F &&f) {
  static_assert(is_tuple_v<Tuple>, "transform only works on tuples");
  return detail::transform_impl(
      tuple, std::forward<F>(f),
      std::make_index_sequence<tuple_size_v<Tuple>>{});
}

template <class TupleA, class TupleB, class F>
constexpr auto transform(TupleA const &a, TupleB const &b, F &&f) {
  static_assert(is_tuple_v<TupleA> && is_tuple_v<TupleB>,
                "transform only works on tuples");
  static_assert(tuple_size_v<TupleA> == tuple_size_v<TupleB>,
                "transform requires equal sizes");
  return detail::transform_impl(
      a, b, std::forward<F>(f),
      std::make_index_sequence<tuple_size_v<TupleA>>{});
}

template <class T> constexpr auto wrap(T const &value) {
  if constexpr (is_tuple_v<T>) {
    return value;
  } else {
    return minicute::tuple<remove_cvref_t<T>>{value};
  }
}

template <class T> constexpr auto unwrap(T const &value) {
  if constexpr (is_tuple_v<T>) {
    if constexpr (tuple_size_v<T> == 1) {
      return get<0>(value);
    } else {
      return value;
    }
  } else {
    return value;
  }
}

template <class Tuple, class X>
constexpr auto append(Tuple const &values, X const &value) {
  static_assert(is_tuple_v<Tuple>, "append only works on tuples");
  return minicute::tuple_cat(
      values, minicute::tuple<remove_cvref_t<X>>{value});
}

template <class Tuple, class X>
constexpr auto prepend(Tuple const &values, X const &value) {
  static_assert(is_tuple_v<Tuple>, "prepend only works on tuples");
  return minicute::tuple_cat(
      minicute::tuple<remove_cvref_t<X>>{value}, values);
}

namespace detail {

template <int I, class Tuple, class X, std::size_t... Is1,
          std::size_t... Is2>
constexpr auto replace_impl(Tuple const &values, X const &value,
                            std::index_sequence<Is1...>,
                            std::index_sequence<Is2...>) {
  return minicute::tuple_cat(
      minicute::tuple<minicute::tuple_element_t<Is1,
                                                remove_cvref_t<Tuple>>...>{
          get<Is1>(values)...},
      minicute::tuple<remove_cvref_t<X>>{value},
      minicute::tuple<minicute::tuple_element_t<
          I + 1 + Is2, remove_cvref_t<Tuple>>...>{get<I + 1 + Is2>(values)...});
}

template <int Begin, int End, class Tuple, std::size_t... Is>
constexpr auto take_impl(Tuple const &values, std::index_sequence<Is...>) {
  return minicute::tuple<
      minicute::tuple_element_t<Begin + Is, remove_cvref_t<Tuple>>...>{
      get<Begin + Is>(values)...};
}

template <class X, std::size_t... Is>
constexpr auto repeat_impl(X const &value, std::index_sequence<Is...>) {
  return minicute::tuple<
      std::conditional_t<true, remove_cvref_t<X>,
                         std::integral_constant<std::size_t, Is>>...>{
                         ((void)Is, value)...};
}

template <class Result>
concept static_predicate_result = requires { Result::value; };

template <class Tuple, class Pred, std::size_t I>
constexpr auto find_if_tuple_impl(Tuple const &values, Pred &&pred) {
  using tuple_type = remove_cvref_t<Tuple>;

  if constexpr (I == tuple_size_v<tuple_type>) {
    return C<I>{};
  } else {
    using result = remove_cvref_t<decltype(
        std::forward<Pred>(pred)(get<I>(values)))>;

    static_assert(static_predicate_result<result>,
                  "find_if requires a static predicate result");

    if constexpr (static_cast<bool>(result::value)) {
      return C<I>{};
    } else {
      return find_if_tuple_impl<Tuple, Pred, I + 1>(
          values, std::forward<Pred>(pred));
    }
  }
}

} // namespace detail

template <int I, class Tuple, class X>
constexpr auto replace(Tuple const &values, X const &value) {
  using T = remove_cvref_t<Tuple>;
  static_assert(is_tuple_v<T>, "replace only works on tuples");

  constexpr int R = static_cast<int>(tuple_size_v<T>);
  static_assert(0 <= I && I < R, "replace index is out of bounds");

  return detail::replace_impl<I>(
      values, value, std::make_index_sequence<I>{},
      std::make_index_sequence<R - I - 1>{});
}

template <int Begin, int End, class Tuple>
constexpr auto take(Tuple const &values) {
  using T = remove_cvref_t<Tuple>;
  static_assert(is_tuple_v<T>, "take only works on tuples");
  static_assert(0 <= Begin && Begin <= End,
                "take requires Begin <= End");

  constexpr int R = static_cast<int>(tuple_size_v<T>);
  static_assert(End <= R, "take range is out of bounds");

  return detail::take_impl<Begin, End>(
      values, std::make_index_sequence<End - Begin>{});
}

template <int... Is, class Tuple>
constexpr auto select(Tuple const &values) {
  using T = remove_cvref_t<Tuple>;
  static_assert(is_tuple_v<T>, "select only works on tuples");
  static_assert(((0 <= Is && Is < static_cast<int>(tuple_size_v<T>)) && ...),
                "select index is out of bounds");

  return minicute::tuple<minicute::tuple_element_t<Is, T>...>{
      get<Is>(values)...};
}

template <int Begin, int End, class Tuple>
constexpr auto group(Tuple const &values) {
  using T = remove_cvref_t<Tuple>;
  static_assert(is_tuple_v<T>, "group only works on tuples");
  static_assert(0 <= Begin && Begin <= End,
                "group requires Begin <= End");

  constexpr int R = static_cast<int>(tuple_size_v<T>);
  static_assert(End <= R, "group range is out of bounds");

  auto middle = take<Begin, End>(values);
  return minicute::tuple_cat(
      take<0, Begin>(values),
      minicute::tuple<remove_cvref_t<decltype(middle)>>{middle},
      take<End, R>(values));
}

template <int N, class X>
constexpr auto repeat(X const &value) {
  static_assert(0 <= N, "repeat requires a non-negative count");
  return detail::repeat_impl(value, std::make_index_sequence<N>{});
}

template <class Guide, class X>
constexpr auto repeat_like(Guide const &guide, X const &value) {
  if constexpr (is_tuple_v<Guide>) {
    return transform(guide, [&](auto const &child) {
      return repeat_like(child, value);
    });
  } else {
    return value;
  }
}

template <int I, class Tuple> constexpr auto remove(Tuple const &values) {
  using T = remove_cvref_t<Tuple>;

  static_assert(is_tuple_v<T>, "remove only works on tuples");

  constexpr int R = static_cast<int>(tuple_size_v<T>);
  static_assert(0 <= I && I < R, "remove index is out of bounds");

  return tuple_cat(take<0, I>(values), take<I + 1, R>(values));
}

template <class T, class Pred>
constexpr auto find_if(T const &value, Pred &&pred) {
  if constexpr (is_tuple_v<T>) {
    return detail::find_if_tuple_impl<T, Pred, 0>(
        value, std::forward<Pred>(pred));
  } else {
    using result = remove_cvref_t<decltype(
        std::forward<Pred>(pred)(value))>;

    static_assert(detail::static_predicate_result<result>,
                  "find_if requires a static predicate result");

    return C<static_cast<bool>(result::value) ? 0 : 1>{};
  }
}

template <class X, class... Ts>
constexpr auto find(minicute::tuple<Ts...> const &) noexcept {
  return C<detail::type_index_v<X, Ts...>>{};
}


template <class... Ts, class Value>
  requires(is_static_integral_v<Value>)
constexpr auto find(minicute::tuple<Ts...> const &values, Value const &) {
  return find<remove_cvref_t<Value>>(values);
}

} // namespace minicute
