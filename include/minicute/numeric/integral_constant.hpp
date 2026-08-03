#pragma once

#include <minicute/util/type_traits.hpp>

#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <type_traits>
#include <utility>

namespace minicute {

// A compile-time value. This is the small integer vocabulary consumed by the
// IntTuple layer and by Layout.
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

using _m32 = Int<-32>;
using _m24 = Int<-24>;
using _m16 = Int<-16>;
using _m12 = Int<-12>;
using _m10 = Int<-10>;
using _m9 = Int<-9>;
using _m8 = Int<-8>;
using _m7 = Int<-7>;
using _m6 = Int<-6>;
using _m5 = Int<-5>;
using _m4 = Int<-4>;
using _m3 = Int<-3>;
using _m2 = Int<-2>;
using _m1 = Int<-1>;

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
using _40 = Int<40>;
using _48 = Int<48>;
using _56 = Int<56>;
using _64 = Int<64>;
using _72 = Int<72>;
using _80 = Int<80>;
using _88 = Int<88>;
using _96 = Int<96>;
using _104 = Int<104>;
using _112 = Int<112>;
using _120 = Int<120>;
using _128 = Int<128>;
using _136 = Int<136>;
using _144 = Int<144>;
using _152 = Int<152>;
using _160 = Int<160>;
using _168 = Int<168>;
using _176 = Int<176>;
using _184 = Int<184>;
using _192 = Int<192>;
using _200 = Int<200>;
using _208 = Int<208>;
using _216 = Int<216>;
using _224 = Int<224>;
using _232 = Int<232>;
using _240 = Int<240>;
using _248 = Int<248>;
using _256 = Int<256>;
using _384 = Int<384>;
using _512 = Int<512>;
using _768 = Int<768>;
using _1024 = Int<1024>;
using _2048 = Int<2048>;
using _4096 = Int<4096>;
using _8192 = Int<8192>;
using _16384 = Int<16384>;
using _32768 = Int<32768>;
using _65536 = Int<65536>;
using _131072 = Int<131072>;
using _262144 = Int<262144>;
using _524288 = Int<524288>;

template <class T, T v>
using constant = C<v>;

template <bool b>
using bool_constant = C<b>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

// The explicitly typed counterpart of C<v>. C<v> remains the compact static
// value used by the layout and IntTuple layers.
template <class T, T v>
struct integral_constant : C<v> {
  using type = integral_constant<T, v>;
  static constexpr T value = v;
  using value_type = T;

  constexpr value_type operator()() const noexcept { return value; }
};

template <class T>
struct is_integral : bool_constant<std::is_integral_v<T>> {};

template <auto V>
struct is_integral<C<V>> : true_type {};

template <class T, T V>
struct is_integral<integral_constant<T, V>> : true_type {};

template <class T>
inline constexpr bool is_integral_v = is_integral<remove_cvref_t<T>>::value;

template <class T>
struct is_static : bool_constant<std::is_empty_v<T>> {};

template <class T>
struct is_static<T const> : is_static<T> {};
template <class T>
struct is_static<T volatile> : is_static<T> {};
template <class T>
struct is_static<T const volatile> : is_static<T> {};
template <class T>
struct is_static<T&> : is_static<T> {};
template <class T>
struct is_static<T const&> : is_static<T> {};
template <class T>
struct is_static<T volatile&> : is_static<T> {};
template <class T>
struct is_static<T const volatile&> : is_static<T> {};
template <class T>
struct is_static<T&&> : is_static<T> {};
template <class T>
struct is_static<T const&&> : is_static<T> {};
template <class T>
struct is_static<T volatile&&> : is_static<T> {};
template <class T>
struct is_static<T const volatile&&> : is_static<T> {};

template <class T>
inline constexpr bool is_static_v = is_static<T>::value;

template <auto N, class T>
struct is_constant : false_type {};

template <auto N, auto V>
struct is_constant<N, C<V>> : bool_constant<(V == N)> {};

template <auto N, class T, T V>
struct is_constant<N, integral_constant<T, V>> : bool_constant<(V == N)> {};

template <auto N, class T>
struct is_constant<N, T const> : is_constant<N, T> {};
template <auto N, class T>
struct is_constant<N, T volatile> : is_constant<N, T> {};
template <auto N, class T>
struct is_constant<N, T const volatile> : is_constant<N, T> {};
template <auto N, class T>
struct is_constant<N, T&> : is_constant<N, T> {};
template <auto N, class T>
struct is_constant<N, T const&> : is_constant<N, T> {};
template <auto N, class T>
struct is_constant<N, T volatile&> : is_constant<N, T> {};
template <auto N, class T>
struct is_constant<N, T const volatile&> : is_constant<N, T> {};
template <auto N, class T>
struct is_constant<N, T&&> : is_constant<N, T> {};
template <auto N, class T>
struct is_constant<N, T const&&> : is_constant<N, T> {};
template <auto N, class T>
struct is_constant<N, T volatile&&> : is_constant<N, T> {};
template <auto N, class T>
struct is_constant<N, T const volatile&&> : is_constant<N, T> {};

template <auto N, class T>
inline constexpr bool is_constant_v = is_constant<N, T>::value;

template <class T>
struct is_static_integral : std::false_type {};

template <auto V>
struct is_static_integral<C<V>> : std::true_type {};

template <class T, T V>
struct is_static_integral<integral_constant<T, V>> : std::true_type {};

template <class T>
inline constexpr bool is_static_integral_v =
    is_static_integral<remove_cvref_t<T>>::value;

// Static unary and binary operations preserve compile-time information.
#define MINICUTE_STATIC_UNARY(OP)                                \
  template <auto A> constexpr auto operator OP(C<A>) {           \
    return C<(OP A)>{};                                          \
  }

#define MINICUTE_STATIC_BINARY(OP)                               \
  template <auto A, auto B> constexpr auto operator OP(C<A>, C<B>) { \
    return C<(A OP B)>{};                                        \
  }

MINICUTE_STATIC_UNARY(+)
MINICUTE_STATIC_UNARY(-)
MINICUTE_STATIC_UNARY(~)
MINICUTE_STATIC_UNARY(!)
MINICUTE_STATIC_UNARY(*)

MINICUTE_STATIC_BINARY(+)
MINICUTE_STATIC_BINARY(-)
MINICUTE_STATIC_BINARY(*)
MINICUTE_STATIC_BINARY(/)
MINICUTE_STATIC_BINARY(%)
MINICUTE_STATIC_BINARY(&)
MINICUTE_STATIC_BINARY(|)
MINICUTE_STATIC_BINARY(^)
MINICUTE_STATIC_BINARY(<<)
MINICUTE_STATIC_BINARY(>>)
MINICUTE_STATIC_BINARY(&&)
MINICUTE_STATIC_BINARY(||)
MINICUTE_STATIC_BINARY(==)
MINICUTE_STATIC_BINARY(!=)
MINICUTE_STATIC_BINARY(<)
MINICUTE_STATIC_BINARY(<=)
MINICUTE_STATIC_BINARY(>)
MINICUTE_STATIC_BINARY(>=)

#undef MINICUTE_STATIC_BINARY
#undef MINICUTE_STATIC_UNARY

namespace detail {

template <class T>
constexpr auto static_abs(T value) {
  if constexpr (std::is_signed_v<T>) {
    return value < T{0} ? -value : value;
  } else {
    return value;
  }
}

template <class T>
constexpr int static_signum(T value) {
  if constexpr (std::is_signed_v<T>) {
    return (T{0} < value) - (value < T{0});
  } else {
    return value != T{0};
  }
}

template <class T>
constexpr bool static_has_single_bit(T value) {
  return value != T{0} && (value & (value - T{1})) == T{0};
}

template <class T, class U>
constexpr auto static_gcd(T a, U b) {
  using common_type = std::common_type_t<T, U>;
  return std::gcd(static_cast<common_type>(a), static_cast<common_type>(b));
}

template <class T, class U>
constexpr auto static_lcm(T a, U b) {
  using common_type = std::common_type_t<T, U>;
  return std::lcm(static_cast<common_type>(a), static_cast<common_type>(b));
}

template <char... Digits>
constexpr std::uint64_t parse_literal_digits() {
  static_assert((('0' <= Digits && Digits <= '9') && ...),
                "minicute::_c expects decimal digits");

  std::uint64_t result = 0;
  ((result = result * 10 + static_cast<unsigned>(Digits - '0')), ...);
  return result;
}

template <char... Digits>
consteval int literal_value() {
  constexpr auto value = parse_literal_digits<Digits...>();
  static_assert(value <= static_cast<std::uint64_t>(std::numeric_limits<int>::max()),
                "minicute::_c literal is too large for int");
  return static_cast<int>(value);
}

} // namespace detail

// Mixed static/dynamic cases whose result is still known at compile time.
template <auto A, class U>
  requires(std::is_integral_v<remove_cvref_t<U>> && A == 0)
constexpr C<0> operator*(C<A>, U) {
  return {};
}

template <class U, auto A>
  requires(std::is_integral_v<remove_cvref_t<U>> && A == 0)
constexpr C<0> operator*(U, C<A>) {
  return {};
}

template <auto A, class U>
  requires(std::is_integral_v<remove_cvref_t<U>> && A == 0)
constexpr C<0> operator/(C<A>, U) {
  return {};
}

template <class U, auto A>
  requires(std::is_integral_v<remove_cvref_t<U>> && (A == 1 || A == -1))
constexpr C<0> operator%(U, C<A>) {
  return {};
}

template <auto A, class U>
  requires(std::is_integral_v<remove_cvref_t<U>> && A == 0)
constexpr C<0> operator%(C<A>, U) {
  return {};
}

template <auto A, class U>
  requires(std::is_integral_v<remove_cvref_t<U>> && A == 0)
constexpr C<0> operator&(C<A>, U) {
  return {};
}

template <class U, auto A>
  requires(std::is_integral_v<remove_cvref_t<U>> && A == 0)
constexpr C<0> operator&(U, C<A>) {
  return {};
}

template <auto A, class U>
  requires(std::is_integral_v<remove_cvref_t<U>> && !static_cast<bool>(A))
constexpr C<false> operator&&(C<A>, U) {
  return {};
}

template <class U, auto A>
  requires(std::is_integral_v<remove_cvref_t<U>> && !static_cast<bool>(A))
constexpr C<false> operator&&(U, C<A>) {
  return {};
}

template <class U, auto A>
  requires(std::is_integral_v<remove_cvref_t<U>> && static_cast<bool>(A))
constexpr C<true> operator||(C<A>, U) {
  return {};
}

template <class U, auto A>
  requires(std::is_integral_v<remove_cvref_t<U>> && static_cast<bool>(A))
constexpr C<true> operator||(U, C<A>) {
  return {};
}

// These arithmetic helpers live here because the pure C++ MiniCuTe build does
// not have a separate CUDA-backed math header yet.
template <class T, class U>
  requires(std::is_arithmetic_v<T> && std::is_arithmetic_v<U>)
constexpr auto max(T const& a, U const& b) {
  using common_type = std::common_type_t<T, U>;
  const auto lhs = static_cast<common_type>(a);
  const auto rhs = static_cast<common_type>(b);
  return lhs < rhs ? rhs : lhs;
}

template <class T, class U>
  requires(std::is_arithmetic_v<T> && std::is_arithmetic_v<U>)
constexpr auto min(T const& a, U const& b) {
  using common_type = std::common_type_t<T, U>;
  const auto lhs = static_cast<common_type>(a);
  const auto rhs = static_cast<common_type>(b);
  return lhs < rhs ? lhs : rhs;
}

template <class T>
  requires(std::is_arithmetic_v<T>)
constexpr auto abs(T value) {
  return detail::static_abs(value);
}

template <class T>
  requires(std::is_arithmetic_v<T>)
constexpr int signum(T value) {
  return detail::static_signum(value);
}

template <class T>
  requires(std::is_integral_v<T>)
constexpr bool has_single_bit(T value) {
  return detail::static_has_single_bit(value);
}

template <class T, class U>
  requires(std::is_integral_v<T> && std::is_integral_v<U>)
constexpr auto gcd(T a, U b) {
  return detail::static_gcd(a, b);
}

template <class T, class U>
  requires(std::is_integral_v<T> && std::is_integral_v<U>)
constexpr auto lcm(T a, U b) {
  return detail::static_lcm(a, b);
}

template <class T>
  requires(std::is_integral_v<T>)
constexpr auto shiftl(T value, int shift) {
  return shift >= 0 ? (value << shift) : (value >> -shift);
}

template <class T>
  requires(std::is_integral_v<T>)
constexpr auto shiftr(T value, int shift) {
  return shift >= 0 ? (value >> shift) : (value << -shift);
}

// Named functions preserve static results for static operands.
template <auto A>
constexpr auto abs(C<A>) {
  return C<detail::static_abs(A)>{};
}

template <auto A>
constexpr auto signum(C<A>) {
  return C<detail::static_signum(A)>{};
}

template <auto A>
constexpr auto has_single_bit(C<A>) {
  return C<detail::static_has_single_bit(A)>{};
}

template <auto A, auto B>
  requires(std::is_arithmetic_v<decltype(A)> &&
           std::is_arithmetic_v<decltype(B)>)
constexpr auto max(C<A>, C<B>) {
  return C<(A < B ? B : A)>{};
}

template <auto A, auto B>
  requires(std::is_arithmetic_v<decltype(A)> &&
           std::is_arithmetic_v<decltype(B)>)
constexpr auto min(C<A>, C<B>) {
  using common_type = std::common_type_t<decltype(A), decltype(B)>;
  return C<(static_cast<common_type>(A) < static_cast<common_type>(B) ? A : B)>{};
}

template <auto A, class U>
  requires(std::is_integral_v<remove_cvref_t<U>>)
constexpr auto max(C<A>, U value) {
  using common_type = std::common_type_t<decltype(A), remove_cvref_t<U>>;
  const auto lhs = static_cast<common_type>(A);
  const auto rhs = static_cast<common_type>(value);
  return lhs < rhs ? rhs : lhs;
}

template <class T, auto B>
  requires(std::is_integral_v<remove_cvref_t<T>>)
constexpr auto max(T value, C<B>) {
  using common_type = std::common_type_t<remove_cvref_t<T>, decltype(B)>;
  const auto lhs = static_cast<common_type>(value);
  const auto rhs = static_cast<common_type>(B);
  return lhs < rhs ? rhs : lhs;
}

template <auto A, class U>
  requires(std::is_integral_v<remove_cvref_t<U>>)
constexpr auto min(C<A>, U value) {
  using common_type = std::common_type_t<decltype(A), remove_cvref_t<U>>;
  const auto lhs = static_cast<common_type>(A);
  const auto rhs = static_cast<common_type>(value);
  return lhs < rhs ? lhs : rhs;
}

template <class T, auto B>
  requires(std::is_integral_v<remove_cvref_t<T>>)
constexpr auto min(T value, C<B>) {
  using common_type = std::common_type_t<remove_cvref_t<T>, decltype(B)>;
  const auto lhs = static_cast<common_type>(value);
  const auto rhs = static_cast<common_type>(B);
  return lhs < rhs ? lhs : rhs;
}

template <auto A, auto B>
constexpr auto shiftl(C<A>, C<B>) {
  return C<(B >= 0 ? (A << B) : (A >> -B))>{};
}

template <auto A, auto B>
constexpr auto shiftr(C<A>, C<B>) {
  return C<(B >= 0 ? (A >> B) : (A << -B))>{};
}

template <auto A, class U>
  requires(std::is_integral_v<remove_cvref_t<U>>)
constexpr auto shiftl(C<A>, U shift) {
  return shift >= 0 ? (A << shift) : (A >> -shift);
}

template <class T, auto B>
  requires(std::is_integral_v<remove_cvref_t<T>>)
constexpr auto shiftl(T value, C<B>) {
  return B >= 0 ? (value << B) : (value >> -B);
}

template <auto A, class U>
  requires(std::is_integral_v<remove_cvref_t<U>>)
constexpr auto shiftr(C<A>, U shift) {
  return shift >= 0 ? (A >> shift) : (A << -shift);
}

template <class T, auto B>
  requires(std::is_integral_v<remove_cvref_t<T>>)
constexpr auto shiftr(T value, C<B>) {
  return B >= 0 ? (value >> B) : (value << -B);
}

template <auto A, auto B>
constexpr auto gcd(C<A>, C<B>) {
  return C<detail::static_gcd(A, B)>{};
}

template <auto A, class U>
  requires(std::is_integral_v<remove_cvref_t<U>>)
constexpr auto gcd(C<A>, U value) {
  return detail::static_gcd(A, value);
}

template <class T, auto B>
  requires(std::is_integral_v<remove_cvref_t<T>>)
constexpr auto gcd(T value, C<B>) {
  return detail::static_gcd(value, B);
}

template <auto A, auto B>
constexpr auto lcm(C<A>, C<B>) {
  return C<detail::static_lcm(A, B)>{};
}

template <auto A, class U>
  requires(std::is_integral_v<remove_cvref_t<U>>)
constexpr auto lcm(C<A>, U value) {
  return detail::static_lcm(A, value);
}

template <class T, auto B>
  requires(std::is_integral_v<remove_cvref_t<T>>)
constexpr auto lcm(T value, C<B>) {
  return detail::static_lcm(value, B);
}

template <class T, class U>
  requires(std::is_integral_v<T> && std::is_integral_v<U>)
constexpr auto safe_div(T const& a, U const& b) {
  return a / b;
}

template <auto A, auto B>
constexpr C<(A / B)> safe_div(C<A>, C<B>) {
  static_assert(A % B == 0, "static safe_div requires exact division");
  return {};
}

template <auto A, class U>
  requires(std::is_integral_v<remove_cvref_t<U>>)
constexpr auto safe_div(C<A>, U value) {
  return A / value;
}

template <class T, auto B>
  requires(std::is_integral_v<remove_cvref_t<T>>)
constexpr auto safe_div(T value, C<B>) {
  return value / B;
}

template <class TrueType, class FalseType>
constexpr decltype(auto) conditional_return(true_type,
                                             TrueType&& true_value,
                                             FalseType&&) {
  return static_cast<TrueType&&>(true_value);
}

template <class TrueType, class FalseType>
constexpr decltype(auto) conditional_return(false_type,
                                             TrueType&&,
                                             FalseType&& false_value) {
  return static_cast<FalseType&&>(false_value);
}

template <auto V>
constexpr auto conditional_return(bool, C<V> const&, C<V> const&) {
  return C<V>{};
}

template <auto V, auto U>
constexpr auto conditional_return(bool condition,
                                  C<V> const& true_value,
                                  C<U> const& false_value) {
  return condition ? static_cast<decltype(V)>(true_value)
                   : static_cast<decltype(U)>(false_value);
}

template <class TrueType, class FalseType>
constexpr auto conditional_return(bool condition,
                                  TrueType const& true_value,
                                  FalseType const& false_value) {
  return condition ? true_value : false_value;
}

template <bool Condition, class TrueType, class FalseType>
constexpr auto conditional_return(TrueType const& true_value,
                                  FalseType const& false_value) {
  if constexpr (Condition) {
    return true_value;
  } else {
    return false_value;
  }
}

template <class Trait>
constexpr auto static_value() {
  if constexpr (std::is_integral_v<decltype(Trait::value)>) {
    return Int<Trait::value>{};
  } else {
    return Trait::value;
  }
}

template <auto Value>
void print(C<Value>) {
  std::cout << "_" << Value;
}

template <auto Value>
std::ostream& operator<<(std::ostream& stream, C<Value> const&) {
  return stream << "_" << Value;
}

template <char... Digits>
constexpr constant<int, detail::literal_value<Digits...>()> operator""_c() {
  return {};
}

} // namespace minicute
