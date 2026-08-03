#pragma once

// This header is the MiniCuTe spelling of CuTe's public type-trait facade.
// The implementation intentionally forwards to the C++ standard library.  It
// exists so code written against cute/util/type_traits.hpp can keep using the
// same names; MiniCuTe internals do not need to depend on this facade.

#include <cstddef>
#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

namespace minicute {

using std::enable_if;
using std::enable_if_t;

}  // namespace minicute

#ifndef __CUTE_REQUIRES
#define __CUTE_REQUIRES(...) \
  typename minicute::enable_if<(__VA_ARGS__)>::type* = nullptr
#endif

#ifndef __CUTE_REQUIRES_V
#define __CUTE_REQUIRES_V(...) \
  typename minicute::enable_if<decltype((__VA_ARGS__))::value>::type* = nullptr
#endif

namespace minicute {

// <type_traits>
using std::conjunction;
using std::conjunction_v;

using std::disjunction;
using std::disjunction_v;

using std::negation;
using std::negation_v;

using std::void_t;
using std::is_void_v;

using std::is_base_of;
using std::is_base_of_v;

using std::is_const;
using std::is_const_v;
using std::is_volatile;
using std::is_volatile_v;

using std::conditional;
using std::conditional_t;

using std::add_const_t;

using std::remove_const_t;
using std::remove_cv_t;
using std::remove_reference_t;

template <class Src, class Dst>
struct copy_cv {
  using type = Dst;
};

template <class Src, class Dst>
struct copy_cv<Src const, Dst> {
  using type = Dst const;
};

template <class Src, class Dst>
struct copy_cv<Src volatile, Dst> {
  using type = Dst volatile;
};

template <class Src, class Dst>
struct copy_cv<Src const volatile, Dst> {
  using type = Dst const volatile;
};

template <class Src, class Dst>
using copy_cv_t = typename copy_cv<Src, Dst>::type;

using std::extent;
using std::remove_extent;

using std::decay;
using std::decay_t;

using std::is_lvalue_reference;
using std::is_lvalue_reference_v;

using std::is_reference;
using std::is_trivially_copyable;

using std::is_convertible;
using std::is_convertible_v;

using std::is_same;
using std::is_same_v;

using std::is_constructible;
using std::is_constructible_v;
using std::is_default_constructible;
using std::is_default_constructible_v;
using std::is_standard_layout;
using std::is_standard_layout_v;

using std::is_arithmetic;
using std::is_unsigned;
using std::is_unsigned_v;
using std::is_signed;
using std::is_signed_v;

using std::make_signed;
using std::make_signed_t;

// The name is deliberately different from minicute::is_integral, which is
// owned by numeric/integral_constant.hpp and recognizes MiniCuTe constants.
template <class T>
using is_std_integral = std::is_integral<T>;

using std::is_empty;
using std::is_empty_v;

using std::invoke_result_t;

using std::common_type;
using std::common_type_t;

using std::remove_pointer;
using std::remove_pointer_t;

using std::add_pointer;
using std::add_pointer_t;

using std::alignment_of;
using std::alignment_of_v;

using std::is_pointer;
using std::is_pointer_v;

// <utility>
using std::declval;

template <class T>
constexpr T&& forward(remove_reference_t<T>& value) noexcept {
  return static_cast<T&&>(value);
}

template <class T>
constexpr T&& forward(remove_reference_t<T>&& value) noexcept {
  static_assert(!is_lvalue_reference_v<T>,
                "T cannot be an lvalue reference (e.g., U&).");
  return static_cast<T&&>(value);
}

template <class T>
constexpr remove_reference_t<T>&& move(T&& value) noexcept {
  return static_cast<remove_reference_t<T>&&>(value);
}

// <limits>, <cstddef>, and <cstdint>
using std::numeric_limits;
using std::ptrdiff_t;
using std::uintptr_t;

// Keep the CuTe names, but use the C++20 standard implementation directly.
using std::remove_cvref;
using std::remove_cvref_t;

template <class...>
inline constexpr bool dependent_false = false;

namespace detail {

template <class...>
inline constexpr bool always_false_v = false;

}  // namespace detail

// CuTe-local tuple traits.  They first accept MiniCuTe's own specializations,
// while the constrained primary specializations bridge ordinary std:: tuple-
// like types.
template <class T, class = void>
struct tuple_size;

namespace detail {

template <class T>
concept has_std_tuple_size = requires {
  std::tuple_size<T>::value;
};

template <std::size_t I, class T>
concept has_std_tuple_element = requires {
  typename std::tuple_element<I, T>::type;
};

}  // namespace detail

template <class T>
  requires detail::has_std_tuple_size<T>
struct tuple_size<T, void>
    : std::integral_constant<std::size_t, std::tuple_size<T>::value> {};

template <class T>
inline constexpr std::size_t tuple_size_v = tuple_size<T>::value;

template <std::size_t I, class T, class = void>
struct tuple_element;

template <std::size_t I, class T>
  requires detail::has_std_tuple_element<I, T>
struct tuple_element<I, T, void>
    : std::tuple_element<I, T> {};

template <std::size_t I, class T>
using tuple_element_t = typename tuple_element<I, T>::type;

// Detection helper kept for CuTe source compatibility.  New MiniCuTe code can
// use a requires-expression directly.
namespace detail {

template <class F, class... Args>
concept valid_invocation = requires {
  declval<F>()(declval<Args>()...);
};

template <class F, class... Args>
constexpr auto is_valid_impl() {
  if constexpr (valid_invocation<F, Args...>) {
    return std::true_type{};
  } else {
    return std::false_type{};
  }
}

template <class F>
struct is_valid_fn {
  template <class... Args>
  constexpr auto operator()(Args&&...) const {
    return is_valid_impl<F, Args&&...>();
  }
};

}  // namespace detail

template <class F>
constexpr auto is_valid(F&&) {
  return detail::is_valid_fn<F&&>{};
}

template <class F, class... Args>
constexpr auto is_valid(F&&, Args&&...) {
  return detail::is_valid_impl<F&&, Args&&...>();
}

template <bool B, template <class...> class True,
          template <class...> class False>
struct conditional_template {
  template <class... U>
  using type = True<U...>;
};

template <template <class...> class True, template <class...> class False>
struct conditional_template<false, True, False> {
  template <class... U>
  using type = False<U...>;
};

template <bool B, template <class...> class True,
          template <class...> class False, class... U>
using conditional_template_t =
    typename conditional_template<B, True, False>::template type<U...>;

// `value` is true exactly when T matches at least one of Us.
template <class T, class... Us>
struct is_any_of {
  static constexpr bool value = (std::is_same_v<T, Us> || ...);
};

template <class T, class... Us>
inline constexpr bool is_any_of_v = is_any_of<T, Us...>::value;

}  // namespace minicute
