#pragma once

#include <minicute/container/detail/tuple_fwd.hpp>

#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace minicute::detail {

template <class T>
inline constexpr bool ebo_storable_v =
    std::is_empty_v<T> && !std::is_final_v<T> && !std::is_reference_v<T> && !std::is_const_v<T> &&
    !std::is_volatile_v<T>;

template <std::size_t I, class T, bool UseEbo = ebo_storable_v<T>> class tuple_leaf;

template <std::size_t I, class T> class tuple_leaf<I, T, false> {
public:
    constexpr tuple_leaf() noexcept(std::is_nothrow_default_constructible_v<T>)
        requires std::default_initializable<T>
        : value_() {}
    constexpr tuple_leaf(const tuple_leaf&) = default;

    constexpr tuple_leaf(tuple_leaf&&)
        requires std::is_move_constructible_v<T>
    = default;

    constexpr tuple_leaf(tuple_leaf&&)
        requires(!std::is_move_constructible_v<T>)
    = delete;

    template <class U>
        requires(std::constructible_from<T, U &&> &&
                 !std::same_as<std::remove_cvref_t<U>, tuple_leaf>)
    constexpr explicit(!std::convertible_to<U&&, T>)
        tuple_leaf(U&& value) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : value_(std::forward<U>(value)) {}

    constexpr T& get() & noexcept {
        return value_;
    }

    constexpr const T& get() const& noexcept {
        return value_;
    }

    constexpr T&& get() && noexcept {
        return static_cast<T&&>(value_);
    }

    constexpr const T&& get() const&& noexcept {
        return static_cast<const T&&>(value_);
    }

private:
    T value_;
};

template <std::size_t I, class T> class tuple_leaf<I, T, true> : T {
public:
    constexpr tuple_leaf() noexcept(std::is_nothrow_default_constructible_v<T>)
        requires std::default_initializable<T>
        : T() {}

    template <class U>
        requires(std::constructible_from<T, U &&> &&
                 !std::same_as<std::remove_cvref_t<U>, tuple_leaf>)
    constexpr explicit(!std::convertible_to<U&&, T>)
        tuple_leaf(U&& value) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : T(std::forward<U>(value)) {}

    constexpr tuple_leaf(const tuple_leaf&) = default;

    constexpr tuple_leaf(tuple_leaf&&)
        requires std::is_move_constructible_v<T>
    = default;

    constexpr tuple_leaf(tuple_leaf&&)
        requires(!std::is_move_constructible_v<T>)
    = delete;

    constexpr T& get() & noexcept {
        return static_cast<T&>(*this);
    }

    constexpr const T& get() const& noexcept {
        return static_cast<const T&>(*this);
    }

    constexpr T&& get() && noexcept {
        return static_cast<T&&>(*this);
    }

    constexpr const T&& get() const&& noexcept {
        return static_cast<const T&&>(*this);
    }
};

template <class IndexSequence, class... Ts> class tuple_impl;

template <std::size_t... Is, class... Ts>
class tuple_impl<std::index_sequence<Is...>, Ts...> : public tuple_leaf<Is, Ts>... {
public:
    constexpr tuple_impl() = default;
    constexpr tuple_impl(const tuple_impl&) = default;
    constexpr tuple_impl(tuple_impl&&)
        requires(std::is_move_constructible_v<Ts> && ...)
    = default;

    constexpr tuple_impl(tuple_impl&&)
        requires(!(std::is_move_constructible_v<Ts> && ...))
    = delete;

    template <class... Us>
        requires(sizeof...(Us) == sizeof...(Ts) && (std::constructible_from<Ts, Us &&> && ...))
    constexpr explicit(!(std::convertible_to<Us&&, Ts> && ...))
        tuple_impl(Us&&... values) noexcept((std::is_nothrow_constructible_v<Ts, Us&&> && ...))
        : tuple_leaf<Is, Ts>(std::forward<Us>(values))... {}

    template <class... Us>
        requires(sizeof...(Us) == sizeof...(Ts) &&
                 !std::same_as<tuple_impl<std::index_sequence<Is...>, Ts...>,
                               tuple_impl<std::index_sequence<Is...>, Us...>> &&
                 (std::constructible_from<Ts, Us&> && ...))
    constexpr explicit(!(std::convertible_to<Us&, Ts> && ...))
        tuple_impl(tuple_impl<std::index_sequence<Is...>, Us...>& other) noexcept(
            (std::is_nothrow_constructible_v<Ts, Us&> && ...))
        : tuple_leaf<Is, Ts>(static_cast<tuple_leaf<Is, Us>&>(other).get())... {}

    template <class... Us>
        requires(sizeof...(Us) == sizeof...(Ts) &&
                 !std::same_as<tuple_impl<std::index_sequence<Is...>, Ts...>,
                               tuple_impl<std::index_sequence<Is...>, Us...>> &&
                 (std::constructible_from<Ts, const Us&> && ...))
    constexpr explicit(!(std::convertible_to<const Us&, Ts> && ...))
        tuple_impl(const tuple_impl<std::index_sequence<Is...>, Us...>& other) noexcept(
            (std::is_nothrow_constructible_v<Ts, const Us&> && ...))
        : tuple_leaf<Is, Ts>(static_cast<const tuple_leaf<Is, Us>&>(other).get())... {}

    template <class... Us>
        requires(sizeof...(Us) == sizeof...(Ts) &&
                 !std::same_as<tuple_impl<std::index_sequence<Is...>, Ts...>,
                               tuple_impl<std::index_sequence<Is...>, Us...>> &&
                 (std::constructible_from<Ts, Us &&> && ...))
    constexpr explicit(!(std::convertible_to<Us&&, Ts> && ...))
        tuple_impl(tuple_impl<std::index_sequence<Is...>, Us...>&& other) noexcept(
            (std::is_nothrow_constructible_v<Ts, Us&&> && ...))
        : tuple_leaf<Is, Ts>(static_cast<tuple_leaf<Is, Us>&&>(other).get())... {}

    template <class... Us>
        requires(sizeof...(Us) == sizeof...(Ts) && (std::is_assignable_v<Ts&, Us&> && ...))
    constexpr void assign_from(tuple_impl<std::index_sequence<Is...>, Us...>& other) noexcept(
        (std::is_nothrow_assignable_v<Ts&, Us&> && ...)) {
        ((static_cast<tuple_leaf<Is, Ts>&>(*this).get() =
              static_cast<tuple_leaf<Is, Us>&>(other).get()),
         ...);
    }

    template <class... Us>
        requires(sizeof...(Us) == sizeof...(Ts) && (std::is_assignable_v<Ts&, const Us&> && ...))
    constexpr void assign_from(const tuple_impl<std::index_sequence<Is...>, Us...>& other) noexcept(
        (std::is_nothrow_assignable_v<Ts&, const Us&> && ...)) {
        ((static_cast<tuple_leaf<Is, Ts>&>(*this).get() =
              static_cast<const tuple_leaf<Is, Us>&>(other).get()),
         ...);
    }

    template <class... Us>
        requires(sizeof...(Us) == sizeof...(Ts) && (std::is_assignable_v<Ts&, Us &&> && ...))
    constexpr void assign_from(tuple_impl<std::index_sequence<Is...>, Us...>&& other) noexcept(
        (std::is_nothrow_assignable_v<Ts&, Us&&> && ...)) {
        ((static_cast<tuple_leaf<Is, Ts>&>(*this).get() =
              static_cast<tuple_leaf<Is, Us>&&>(other).get()),
         ...);
    }
};

} // namespace minicute::detail

namespace minicute {

template <class... Ts>
class tuple : public detail::tuple_impl<std::index_sequence_for<Ts...>, Ts...> {
public:
    using base_type = detail::tuple_impl<std::index_sequence_for<Ts...>, Ts...>;

    constexpr tuple() = default;
    constexpr tuple(const tuple&) = default;
    constexpr tuple(tuple&&)
        requires(std::is_move_constructible_v<Ts> && ...)
    = default;

    constexpr tuple(tuple&&)
        requires(!(std::is_move_constructible_v<Ts> && ...))
    = delete;

    template <class... Us>
        requires((sizeof...(Us) == sizeof...(Ts)) && (std::constructible_from<Ts, Us &&> && ...))
    constexpr explicit(!(std::convertible_to<Us&&, Ts> && ...))
        tuple(Us&&... values) noexcept((std::is_nothrow_constructible_v<Ts, Us&&> && ...))
        : base_type(std::forward<Us>(values)...) {}

    template <class... Us>
        requires(sizeof...(Us) == sizeof...(Ts) && !std::same_as<tuple<Ts...>, tuple<Us...>> &&
                 (std::constructible_from<Ts, Us&> && ...))
    constexpr explicit(!(std::convertible_to<Us&, Ts> && ...))
        tuple(tuple<Us...>& other) noexcept((std::is_nothrow_constructible_v<Ts, Us&> && ...))
        : base_type(static_cast<typename tuple<Us...>::base_type&>(other)) {}

    template <class... Us>
        requires(sizeof...(Us) == sizeof...(Ts) && !std::same_as<tuple<Ts...>, tuple<Us...>> &&
                 (std::constructible_from<Ts, const Us&> && ...))
    constexpr explicit(!(std::convertible_to<const Us&, Ts> && ...))
        tuple(const tuple<Us...>& other) noexcept((std::is_nothrow_constructible_v<Ts, const Us&> &&
                                                   ...))
        : base_type(static_cast<const typename tuple<Us...>::base_type&>(other)) {}

    template <class... Us>
        requires(sizeof...(Us) == sizeof...(Ts) && !std::same_as<tuple<Ts...>, tuple<Us...>> &&
                 (std::constructible_from<Ts, Us &&> && ...))
    constexpr explicit(!(std::convertible_to<Us&&, Ts> && ...))
        tuple(tuple<Us...>&& other) noexcept((std::is_nothrow_constructible_v<Ts, Us&&> && ...))
        : base_type(static_cast<typename tuple<Us...>::base_type&&>(other)) {}

    constexpr tuple&
    operator=(const tuple& other) noexcept((std::is_nothrow_assignable_v<Ts&, const Ts&> && ...))
        requires(std::is_assignable_v<Ts&, const Ts&> && ...)
    {
        static_cast<base_type&>(*this).assign_from(static_cast<const base_type&>(other));
        return *this;
    }

    constexpr tuple& operator=(tuple&& other) noexcept((std::is_nothrow_assignable_v<Ts&, Ts&&> &&
                                                        ...))
        requires(std::is_assignable_v<Ts&, Ts &&> && ...)
    {
        static_cast<base_type&>(*this).assign_from(static_cast<base_type&&>(other));
        return *this;
    }

    constexpr tuple& operator=(tuple&&)
        requires(!(std::is_assignable_v<Ts&, Ts &&> && ...))
    = delete;

    template <class... Us>
        requires(sizeof...(Us) == sizeof...(Ts) && (std::is_assignable_v<Ts&, const Us&> && ...))
    constexpr tuple& operator=(const tuple<Us...>& other) noexcept(
        (std::is_nothrow_assignable_v<Ts&, const Us&> && ...)) {
        static_cast<base_type&>(*this).assign_from(
            static_cast<const typename tuple<Us...>::base_type&>(other));
        return *this;
    }

    template <class... Us>
        requires(sizeof...(Us) == sizeof...(Ts) && (std::is_assignable_v<Ts&, Us &&> && ...))
    constexpr tuple&
    operator=(tuple<Us...>&& other) noexcept((std::is_nothrow_assignable_v<Ts&, Us&&> && ...)) {
        static_cast<base_type&>(*this).assign_from(
            static_cast<typename tuple<Us...>::base_type&&>(other));
        return *this;
    }
};

template <class... Ts> tuple(Ts...) -> tuple<Ts...>;

} // namespace minicute
