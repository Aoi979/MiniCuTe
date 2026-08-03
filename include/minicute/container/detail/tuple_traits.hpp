#pragma once

#include <minicute/container/detail/tuple_fwd.hpp>

#include <cstddef>
#include <tuple>
#include <type_traits>

namespace minicute::detail {

template <std::size_t I, class... Ts> struct type_at;

template <std::size_t I, class... Ts> using type_at_t = typename type_at<I, Ts...>::type;

template <class T, class... Ts> struct type_at<0, T, Ts...> {
    using type = T;
};

template <std::size_t I, class T, class... Ts>
struct type_at<I, T, Ts...> : type_at<I - 1, Ts...> {};

template <std::size_t I> struct type_at<I> {
    static_assert(always_false_v<std::integral_constant<std::size_t, I>>,
                  "minicute::tuple index is out of bounds");
};

} // namespace minicute::detail

namespace minicute {

template <class T> struct is_tuple : std::false_type {};

template <class... Ts> struct is_tuple<tuple<Ts...>> : std::true_type {};

template <class T>
inline constexpr bool is_tuple_v = is_tuple<remove_cvref_t<T>>::value;

template <class... Ts> struct tuple_size<tuple<Ts...>> {
    static constexpr std::size_t value = sizeof...(Ts);
};

template <class T> struct tuple_size<const T> {
    static constexpr std::size_t value = tuple_size<T>::value;
};

template <class T> struct tuple_size<volatile T> {
    static constexpr std::size_t value = tuple_size<T>::value;
};

template <class T> struct tuple_size<const volatile T> {
    static constexpr std::size_t value = tuple_size<T>::value;
};

template <std::size_t I, class... Ts> struct tuple_element<I, tuple<Ts...>> {
    using type = detail::type_at_t<I, Ts...>;
};

template <std::size_t I, class T> struct tuple_element<I, const T> {
    using type = const typename tuple_element<I, T>::type;
};

template <std::size_t I, class T> struct tuple_element<I, volatile T> {
    using type = volatile typename tuple_element<I, T>::type;
};

template <std::size_t I, class T> struct tuple_element<I, const volatile T> {
    using type = const volatile typename tuple_element<I, T>::type;
};

} // namespace minicute

namespace std {

template <class... Ts>
struct tuple_size<minicute::tuple<Ts...>> : minicute::tuple_size<minicute::tuple<Ts...>> {};

template <std::size_t I, class... Ts>
struct tuple_element<I, minicute::tuple<Ts...>> : minicute::tuple_element<I, minicute::tuple<Ts...>> {};

} // namespace std
