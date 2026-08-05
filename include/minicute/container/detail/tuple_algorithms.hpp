#pragma once

#include <minicute/container/detail/tuple_access.hpp>
#include <minicute/container/detail/tuple_fwd.hpp>
#include <minicute/container/detail/tuple_traits.hpp>

#include <type_traits>
#include <utility>

namespace minicute::detail {

template <class T> struct unwrap_reference_wrapper {
    using type = T;
};

template <class T> struct unwrap_reference_wrapper<std::reference_wrapper<T>> {
    using type = T&;
};

template <class T> using unwrap_decay_t = typename unwrap_reference_wrapper<std::decay_t<T>>::type;

template <class Tuple> using tuple_object_t = std::remove_cvref_t<Tuple>;

template <class First, class Second, std::size_t... FirstIs, std::size_t... SecondIs>
constexpr auto tuple_cat_two_impl(First&& first, Second&& second, std::index_sequence<FirstIs...>,
                                  std::index_sequence<SecondIs...>) {
    using first_tuple = tuple_object_t<First>;
    using second_tuple = tuple_object_t<Second>;
    using result_type =
        tuple<tuple_element_t<FirstIs, first_tuple>..., tuple_element_t<SecondIs, second_tuple>...>;

    return result_type(minicute::get<FirstIs>(std::forward<First>(first))...,
                       minicute::get<SecondIs>(std::forward<Second>(second))...);
}

template <class First, class Second> constexpr auto tuple_cat_two(First&& first, Second&& second) {
    using first_tuple = tuple_object_t<First>;
    using second_tuple = tuple_object_t<Second>;

    return tuple_cat_two_impl(std::forward<First>(first), std::forward<Second>(second),
                              std::make_index_sequence<tuple_size_v<first_tuple>>{},
                              std::make_index_sequence<tuple_size_v<second_tuple>>{});
}

template <class Tuple> constexpr auto tuple_cat_one(Tuple&& t) {
    return tuple_cat_two(tuple<>{}, std::forward<Tuple>(t));
}

} // namespace minicute::detail

namespace minicute {

template <class... Ts> constexpr auto make_tuple(Ts&&... values) {
    return tuple<detail::unwrap_decay_t<Ts>...>(std::forward<Ts>(values)...);
}

template <class... Ts> constexpr auto tie(Ts&... values) noexcept {
    return tuple<Ts&...>{values...};
}

template <class... Ts> constexpr auto forward_as_tuple(Ts&&... values) noexcept {
    return tuple<Ts&&...>{std::forward<Ts>(values)...};
}

constexpr tuple<> tuple_cat() noexcept {
    return {};
}

template <class Tuple> constexpr auto tuple_cat(Tuple&& t) {
    return detail::tuple_cat_one(std::forward<Tuple>(t));
}

template <class First, class Second, class... Rest>
constexpr auto tuple_cat(First&& first, Second&& second, Rest&&... rest) {
    auto joined = detail::tuple_cat_two(std::forward<First>(first), std::forward<Second>(second));

    if constexpr (sizeof...(Rest) == 0) {
        return joined;
    } else {
        return tuple_cat(std::move(joined), std::forward<Rest>(rest)...);
  }
}

} // namespace minicute
