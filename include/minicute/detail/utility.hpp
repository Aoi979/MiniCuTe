#pragma once

#include <type_traits>

namespace minicute {

template <class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

namespace detail {

template <class...>
inline constexpr bool always_false_v = false;

}  // namespace detail

}  // namespace minicute
