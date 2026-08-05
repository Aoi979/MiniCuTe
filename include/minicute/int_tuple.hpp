#pragma once

#include "algorithm/tuple_algorithms.hpp"
#include "container/tuple.hpp"
#include <minicute/numeric/integral_constant.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace minicute {

// An IntTuple is either an integer leaf or a nested minicute::tuple of
// IntTuples.  This file contains recursive operations on that structure;
// generic tuple algorithms live in algorithm/tuple_algorithms.hpp, while the
// tuple container itself lives in container/tuple.hpp.
template <class... Ts> using Shape = tuple<Ts...>;

template <class... Ts> using Stride = tuple<Ts...>;

template <class... Ts> using Coord = tuple<Ts...>;

template <class... Ts> using Tile = tuple<Ts...>;

template <class... Ts> constexpr auto make_shape(Ts const &...xs) {
  return Shape<remove_cvref_t<Ts>...>{xs...};
}

template <class... Ts> constexpr auto make_stride(Ts const &...xs) {
  return Stride<remove_cvref_t<Ts>...>{xs...};
}

template <class... Ts> constexpr auto make_coord(Ts const &...xs) {
  return Coord<remove_cvref_t<Ts>...>{xs...};
}

template <class... Ts> constexpr auto make_tile(Ts const &...xs) {
  return Tile<remove_cvref_t<Ts>...>{xs...};
}

template <std::size_t I, class T>
constexpr decltype(auto) get(T &&value) noexcept {
  if constexpr (is_tuple_v<T>) {
    return minicute::get<I>(std::forward<T>(value));
  } else {
    static_assert(I == 0, "scalar values only have get<0>");
    return std::forward<T>(value);
  }
}

template <std::size_t I0, std::size_t I1, std::size_t... Is, class T>
constexpr decltype(auto) get(T &&value) noexcept {
  return get<I1, Is...>(get<I0>(std::forward<T>(value)));
}

template <class T0, class... Ts>
constexpr auto min(T0 const &first, Ts const &...rest) {
  if constexpr (is_tuple_v<T0>) {
    using Tuple = remove_cvref_t<T0>;

    static_assert(tuple_size_v<Tuple> > 0,
                  "min of an empty tuple is undefined");

    auto reduced =
        apply(first, [](auto const &...values) { return min(values...); });

    if constexpr (sizeof...(Ts) == 0) {
      return reduced;
    } else {
      return min(reduced, rest...);
    }
  } else if constexpr (sizeof...(Ts) == 0) {
    return first;
  } else {
    return min(first, min(rest...));
  }
}

template <class T0, class... Ts>
constexpr auto max(T0 const &first, Ts const &...rest) {
  if constexpr (is_tuple_v<T0>) {
    using Tuple = remove_cvref_t<T0>;

    static_assert(tuple_size_v<Tuple> > 0,
                  "max of an empty tuple is undefined");

    auto reduced =
        apply(first, [](auto const &...values) { return max(values...); });

    if constexpr (sizeof...(Ts) == 0) {
      return reduced;
    } else {
      return max(reduced, rest...);
    }
  } else if constexpr (sizeof...(Ts) == 0) {
    return first;
  } else {
    return max(first, max(rest...));
  }
}

template <auto A, auto B> constexpr auto max(C<A>, C<B>) {
  return C<(A > B ? A : B)>{};
}

template <auto A, auto B> constexpr auto min(C<A>, C<B>) {
  return C<(A < B ? A : B)>{};
}

template <class T0, class T1, class... Ts>
constexpr auto max(T0 a, T1 b, Ts... rest) {
  return max(max(a, b), rest...);
}

template <class T> constexpr auto rank(T const &) {
  if constexpr (is_tuple_v<T>) {
    return Int<static_cast<int>(tuple_size_v<T>)>{};
  } else {
    return _1{};
  }
}

template <class T> constexpr auto depth(T const &t) {
  if constexpr (is_tuple_v<T>) {
    if constexpr (tuple_size_v<T> == 0) {
      return _1{};
    } else {
      return apply(t,
                   [&](auto const &...x) { return max(depth(x)...) + _1{}; });
    }
  } else {
    return _0{};
  }
}

template <class T> constexpr auto product(T const &t) {
  if constexpr (is_tuple_v<T>) {
    if constexpr (tuple_size_v<T> == 0) {
      return _1{};
    } else {
      return apply(t,
                   [&](auto const &...x) { return (product(x) * ... * _1{}); });
    }
  } else {
    return t;
  }
}

template <class T> constexpr auto size(T const &value) {
  return product(value);
}

template <class T> constexpr auto sum(T const &t) {
  if constexpr (is_tuple_v<T>) {
    if constexpr (tuple_size_v<T> == 0) {
      return _0{};
    } else {
      return apply(t, [&](auto const &...x) { return (sum(x) + ... + _0{}); });
    }
  } else {
    return t;
  }
}

template <class A, class B>
constexpr auto inner_product(A const &a, B const &b) {
  if constexpr (is_tuple_v<A> && is_tuple_v<B>) {
    static_assert(tuple_size_v<A> == tuple_size_v<B>,
                  "inner_product requires equal sizes");
    if constexpr (tuple_size_v<A> == 0) {
      return _0{};
    } else {
      return sum(transform(a, b, [](auto const &x, auto const &y) {
        return inner_product(x, y);
      }));
    }
  } else {
    return a * b;
  }
}

template <class T> constexpr auto flatten(T const &t) {
  if constexpr (is_tuple_v<T>) {
    if constexpr (tuple_size_v<T> == 0) {
      return minicute::tuple<>{};
    } else {
      return apply(t, [](auto const &...x) {
        return minicute::tuple_cat(flatten(x)...);
      });
    }
  } else {
    return minicute::tuple<remove_cvref_t<T>>{t};
  }
}

namespace detail {
template <class T> struct flat_size : std::integral_constant<std::size_t, 1> {};

template <class... Ts>
struct flat_size<minicute::tuple<Ts...>>
    : std::integral_constant<std::size_t, (flat_size<Ts>::value + ... + 0)> {};

template <class T>
inline constexpr std::size_t flat_size_v = flat_size<remove_cvref_t<T>>::value;

template <std::size_t I, class Tuple, std::size_t... Js>
constexpr std::size_t prefix_flat_size(std::index_sequence<Js...>) {
  return (std::size_t{0} + ... +
          flat_size_v<minicute::tuple_element_t<Js, remove_cvref_t<Tuple>>>);
}

template <std::size_t I, class Tuple>
inline constexpr std::size_t prefix_flat_size_v =
    prefix_flat_size<I, Tuple>(std::make_index_sequence<I>{});

template <std::size_t Offset, class Flat, class Profile>
constexpr auto unflatten_at(Flat const &f, Profile const &p);

template <std::size_t Offset, class Flat, class Profile, std::size_t... Is>
constexpr auto unflatten_at_impl(Flat const &f, Profile const &p,
                                 std::index_sequence<Is...>) {
  return minicute::tuple<remove_cvref_t<
      decltype(unflatten_at<Offset + prefix_flat_size_v<Is, Profile>>(
          f, get<Is>(p)))>...>{
      unflatten_at<Offset + prefix_flat_size_v<Is, Profile>>(f, get<Is>(p))...};
}

template <std::size_t Offset, class Flat, class Profile>
constexpr auto unflatten_at(Flat const &f, Profile const &p) {
  if constexpr (is_tuple_v<Profile>) {
    return unflatten_at_impl<Offset>(
        f, p, std::make_index_sequence<tuple_size_v<Profile>>{});
  } else {
    return get<Offset>(f);
  }
}
} // namespace detail

template <class Flat, class Profile>
constexpr auto unflatten(Flat const &f, Profile const &p) {
  static_assert(is_tuple_v<Flat>, "unflatten requires a flat tuple");
  if constexpr (is_tuple_v<Flat>) {
    static_assert(tuple_size_v<Flat> == detail::flat_size_v<Profile>,
                  "unflatten requires flat/profile sizes to match");
    return detail::unflatten_at<0>(f, p);
  }
}

template <class Tuple> constexpr auto product_each(Tuple const &tuple) {
  static_assert(is_tuple_v<Tuple>, "product_each only works on tuples");
  return transform(tuple, [](auto const &x) { return product(x); });
}

template <class A, class B> constexpr auto ceil_div(A const &a, B const &b) {
  return (a + b - _1{}) / b;
}

template <class A, class B> constexpr auto shape_div(A const &a, B const &b) {
  if constexpr (is_tuple_v<A> && is_tuple_v<B>) {
    static_assert(tuple_size_v<A> == tuple_size_v<B>,
                  "shape_div requires equal sizes");
    return transform(
        a, b, [](auto const &x, auto const &y) { return shape_div(x, y); });
  } else {
    return ceil_div(a, b);
  }
}

template <class A, class B>
constexpr auto elem_product(A const &a, B const &b) {
  if constexpr (is_tuple_v<A> && is_tuple_v<B>) {
    static_assert(tuple_size_v<A> == tuple_size_v<B>,
                  "elem_product requires equal sizes");
    return transform(
        a, b, [](auto const &x, auto const &y) { return elem_product(x, y); });
  } else if constexpr (is_tuple_v<A>) {
    return transform(a, [&](auto const &x) { return elem_product(x, b); });
  } else if constexpr (is_tuple_v<B>) {
    return transform(b, [&](auto const &y) { return elem_product(a, y); });
  } else {
    return a * b;
  }
}

namespace detail {
template <class A, class B>
struct same_profile : std::bool_constant<!is_tuple_v<A> && !is_tuple_v<B>> {};

template <class A, class B, bool SameRank>
struct same_tuple_profile_impl : std::false_type {};

template <class... As, class... Bs>
struct same_tuple_profile_impl<minicute::tuple<As...>, minicute::tuple<Bs...>,
                               true>
    : std::bool_constant<(same_profile<As, Bs>::value && ...)> {};

template <class... As, class... Bs>
struct same_profile<minicute::tuple<As...>, minicute::tuple<Bs...>>
    : same_tuple_profile_impl<minicute::tuple<As...>, minicute::tuple<Bs...>,
                              sizeof...(As) == sizeof...(Bs)> {};

template <class A, class B>
inline constexpr bool same_profile_v =
    same_profile<remove_cvref_t<A>, remove_cvref_t<B>>::value;
} // namespace detail

template <class A, class B> constexpr auto congruent(A const &, B const &) {
  return std::bool_constant<detail::same_profile_v<A, B>>{};
}

template <class A, class B> constexpr auto compatible(A const &a, B const &b) {
  if constexpr (is_tuple_v<A> && is_tuple_v<B>) {
    if constexpr (tuple_size_v<A> != tuple_size_v<B>) {
      return false;
    } else {
      return minicute::apply(
          transform(
              a, b,
              [](auto const &x, auto const &y) { return compatible(x, y); }),
          [](auto const &...xs) { return (true && ... && bool(xs)); });
    }
  } else if constexpr (!is_tuple_v<A>) {
    return a == size(b);
  } else {
    return false;
  }
}

} // namespace minicute
