#pragma once

// CuTe-compatible integer-sequence names.  The sequence representation and
// the standard generators are deliberately borrowed from the C++20 library;
// only CuTe's range/reverse/tuple adapters are defined here.

#include <minicute/numeric/integral_constant.hpp>

#include <cstddef>
#include <utility>

namespace minicute {

using std::integer_sequence;
using std::make_integer_sequence;

namespace detail {

template <class T, class Sequence, T Begin>
struct integer_range_impl;

template <class T, T... Values, T Begin>
struct integer_range_impl<T, integer_sequence<T, Values...>, Begin> {
  using type = integer_sequence<T, static_cast<T>(Values + Begin)...>;
};

template <class Sequence>
struct reverse_integer_sequence_impl;

template <class T, T... Values>
struct reverse_integer_sequence_impl<integer_sequence<T, Values...>> {
  using type = integer_sequence<
      T, static_cast<T>(sizeof...(Values) - 1 - Values)...>;
};

template <std::size_t I, class T, T... Values>
struct integer_sequence_element;

template <std::size_t I, class T, T First, T... Rest>
struct integer_sequence_element<I, T, First, Rest...>
    : integer_sequence_element<I - 1, T, Rest...> {};

template <class T, T First, T... Rest>
struct integer_sequence_element<0, T, First, Rest...> {
  using type = integral_constant<T, First>;
};

template <std::size_t I, class T>
struct integer_sequence_element<I, T> {
  static_assert(always_false_v<std::integral_constant<std::size_t, I>>,
                "minicute::integer_sequence index is out of bounds");
};

template <std::size_t I, class T, T... Values>
using integer_sequence_element_t =
    typename integer_sequence_element<I, T, Values...>::type;

}  // namespace detail

template <class T, T Begin, T End>
using make_integer_range = typename detail::integer_range_impl<
    T, make_integer_sequence<T, (End > Begin ? End - Begin : T{0})>,
    Begin>::type;

template <class T, T N>
using make_integer_sequence_reverse = typename
    detail::reverse_integer_sequence_impl<make_integer_sequence<T, N>>::type;

// Integer sequences.
template <int... Values>
using int_sequence = integer_sequence<int, Values...>;

template <int N>
using make_int_sequence = make_integer_sequence<int, N>;

template <int N>
using make_int_rsequence = make_integer_sequence_reverse<int, N>;

template <int Begin, int End>
using make_int_range = make_integer_range<int, Begin, End>;

// Index sequences.
using std::index_sequence;
using std::make_index_sequence;

template <std::size_t N>
using make_index_rsequence = make_integer_sequence_reverse<std::size_t, N>;

template <std::size_t Begin, std::size_t End>
using make_index_range = make_integer_range<std::size_t, Begin, End>;

// Shortcuts used by CuTe algorithms.
template <int... Values>
using seq = int_sequence<Values...>;

template <int N>
using make_seq = make_int_sequence<N>;

template <int N>
using make_rseq = make_int_rsequence<N>;

template <int Begin, int End>
using make_range = make_int_range<Begin, End>;

template <class Tuple>
using tuple_seq = make_seq<static_cast<int>(
    tuple_size<remove_cvref_t<Tuple>>::value)>;

template <class Tuple>
using tuple_rseq = make_rseq<static_cast<int>(
    tuple_size<remove_cvref_t<Tuple>>::value)>;

// Convert a static integer tuple or an integer sequence to an int sequence.
template <class T>
struct to_seq;

template <>
struct to_seq<integer_sequence<int>> {
  using type = seq<>;
};

template <int First, int... Rest>
struct to_seq<integer_sequence<int, First, Rest...>> {
  using type = seq<First, Rest...>;
};

template <template <class...> class TupleLike, class... Ts>
struct to_seq<TupleLike<Ts...>> {
  using type = seq<Ts::value...>;
};

template <class T>
using to_seq_t = typename to_seq<T>::type;

// Make std::integer_sequence usable through MiniCuTe's tuple protocol.
template <class T, T... Values>
struct tuple_size<integer_sequence<T, Values...>, void>
    : integral_constant<std::size_t, sizeof...(Values)> {};

template <std::size_t I, class T, T... Values>
struct tuple_element<I, integer_sequence<T, Values...>, void> {
  using type = detail::integer_sequence_element_t<I, T, Values...>;
};

template <std::size_t I, class T, T... Values>
constexpr tuple_element_t<I, integer_sequence<T, Values...>>
get(integer_sequence<T, Values...>) {
  static_assert(I < sizeof...(Values),
                "minicute::integer_sequence index is out of bounds");
  return {};
}

}  // namespace minicute
