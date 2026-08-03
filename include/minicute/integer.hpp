#pragma once

#include "detail/utility.hpp"

#include <type_traits>

namespace minicute {

// A compile-time value.  This is the small integer vocabulary consumed by
// the IntTuple layer and by Layout.
template <auto V>
struct C {
  using value_type = decltype(V);
  using type = C<V>;
  static constexpr value_type value = V;

  constexpr operator value_type() const noexcept { return value; }
  constexpr value_type operator()() const noexcept { return value; }
};

template <int N>
using Int = C<N>;

using _0 = Int<0>;
using _1 = Int<1>;
using _2 = Int<2>;
using _3 = Int<3>;
using _4 = Int<4>;
using _5 = Int<5>;
using _6 = Int<6>;
using _7 = Int<7>;
using _8 = Int<8>;
using _9 = Int<9>;
using _10 = Int<10>;
using _11 = Int<11>;
using _12 = Int<12>;
using _13 = Int<13>;
using _14 = Int<14>;
using _15 = Int<15>;
using _16 = Int<16>;
using _20 = Int<20>;
using _23 = Int<23>;
using _24 = Int<24>;
using _32 = Int<32>;
using _64 = Int<64>;
using _128 = Int<128>;

template <class T>
struct is_static_integral : std::false_type {};

template <auto V>
struct is_static_integral<C<V>> : std::true_type {};

template <class T>
inline constexpr bool is_static_integral_v =
    is_static_integral<remove_cvref_t<T>>::value;

template <class T>
inline constexpr bool is_integral_v =
    std::is_integral_v<remove_cvref_t<T>> || is_static_integral_v<T>;

// Static arithmetic/comparison for C<V>.  Mixed static/dynamic arithmetic is
// intentionally still part of the later implementation stages.
#define MINICUTE_STATIC_BINARY(OP)                                            \
  template <auto A, auto B> constexpr auto operator OP(C<A>, C<B>) {          \
    return C<(A OP B)>{};                                                      \
  }

MINICUTE_STATIC_BINARY(+)
MINICUTE_STATIC_BINARY(-)
MINICUTE_STATIC_BINARY(*)
MINICUTE_STATIC_BINARY(/)
MINICUTE_STATIC_BINARY(%)
MINICUTE_STATIC_BINARY(==)
MINICUTE_STATIC_BINARY(!=)
MINICUTE_STATIC_BINARY(<)
MINICUTE_STATIC_BINARY(<=)
MINICUTE_STATIC_BINARY(>)
MINICUTE_STATIC_BINARY(>=)

#undef MINICUTE_STATIC_BINARY

}  // namespace minicute
