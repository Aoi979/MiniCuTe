#pragma once

#include <cstddef>
#include <minicute/int_tuple.hpp>
#include <utility>

namespace minicute {

struct LayoutLeft {};
struct LayoutRight {};
struct underscore_t {};
inline constexpr underscore_t _{};

template <class Shape, class Current = _1>
constexpr auto compact_col_major(Shape const &shape,
                                 Current const &current = {});

namespace detail {
template <std::size_t I, class Shape, std::size_t... Is>
constexpr auto col_major_prefix(Shape const &shape,
                                std::index_sequence<Is...>) {
  return (product(get<Is>(shape)) * ... * _1{});
}

template <std::size_t I, class Shape>
constexpr auto col_major_prefix(Shape const &shape) {
  return col_major_prefix<I>(shape, std::make_index_sequence<I>{});
}

template <class Shape, class Current, std::size_t... Is>
constexpr auto compact_col_major_impl(Shape const &shape,
                                      Current const &current,
                                      std::index_sequence<Is...>) {
  return make_stride(compact_col_major(
      get<Is>(shape), current * col_major_prefix<Is>(shape))...);
}
} // namespace detail

template <class Shape, class Current>
constexpr auto compact_col_major(Shape const &shape, Current const &current) {
  if constexpr (is_tuple_v<Shape>) {
    return detail::compact_col_major_impl(
        shape, current, std::make_index_sequence<tuple_size_v<Shape>>{});
  } else {
    return current;
  }
}

template <class Shape, class Current = _1>
constexpr auto compact_row_major(Shape const &shape,
                                 Current const &current = {});

namespace detail {
template <std::size_t I, class Shape, std::size_t... Is>
constexpr auto row_major_suffix(Shape const &shape,
                                std::index_sequence<Is...>) {
  return (product(get<I + 1 + Is>(shape)) * ... * _1{});
}

template <std::size_t I, class Shape>
constexpr auto row_major_suffix(Shape const &shape) {
  return row_major_suffix<I>(
      shape, std::make_index_sequence<tuple_size_v<Shape> - I - 1>{});
}

template <class Shape, class Current, std::size_t... Is>
constexpr auto compact_row_major_impl(Shape const &shape,
                                      Current const &current,
                                      std::index_sequence<Is...>) {
  return make_stride(compact_row_major(
      get<Is>(shape), current * row_major_suffix<Is>(shape))...);
}
} // namespace detail

template <class Shape, class Current>
constexpr auto compact_row_major(Shape const &shape, Current const &current) {
  if constexpr (is_tuple_v<Shape>) {
    return detail::compact_row_major_impl(
        shape, current, std::make_index_sequence<tuple_size_v<Shape>>{});
  } else {
    return current;
  }
}

template <class Coord, class Shape, class Stride>
constexpr auto crd2idx(Coord const &coord, Shape const &shape,
                       Stride const &stride);

template <class Shape_, class Stride_ = void> struct Layout {
  using Shape = Shape_;
  using Stride = Stride_;

  constexpr Layout(Shape const &shape = {}, Stride const &stride = {})
      : shape_(shape), stride_(stride) {}

  template <std::size_t... Is> constexpr decltype(auto) shape() const {
    if constexpr (sizeof...(Is) == 0) {
      return (shape_);
    } else {
      return get<Is...>(shape_);
    }
  }

  template <std::size_t... Is> constexpr decltype(auto) stride() const {
    if constexpr (sizeof...(Is) == 0) {
      return (stride_);
    } else {
      return get<Is...>(stride_);
    }
  }

  template <class Coord> constexpr auto operator()(Coord const &coord) const {
    return crd2idx(coord, shape_, stride_);
  }

  template <class Coord0, class Coord1, class... Coords>
  constexpr auto operator()(Coord0 const &c0, Coord1 const &c1,
                            Coords const &...cs) const {
    return (*this)(make_coord(c0, c1, cs...));
  }

  Shape shape_;
  Stride stride_;
};

template <class T> struct is_layout : std::false_type {};

template <class Shape, class Stride>
struct is_layout<Layout<Shape, Stride>> : std::true_type {};

template <class T>
inline constexpr bool is_layout_v = is_layout<remove_cvref_t<T>>::value;

template <class Shape, class Stride>
constexpr auto make_layout(Shape const &shape, Stride const &stride) {
  return Layout<remove_cvref_t<Shape>, remove_cvref_t<Stride>>{shape, stride};
}

template <class Shape, class Stride>
constexpr auto make_layout(Layout<Shape, Stride> const &layout) {
  return make_layout(make_shape(layout.shape()), make_stride(layout.stride()));
}

template <class Shape0, class Stride0, class Shape1, class Stride1>
constexpr auto make_layout(Layout<Shape0, Stride0> const &layout0,
                           Layout<Shape1, Stride1> const &layout1) {
  return make_layout(make_shape(layout0.shape(), layout1.shape()),
                     make_stride(layout0.stride(), layout1.stride()));
}

template <class Shape0, class Stride0, class Shape1, class Stride1,
          class... Layouts>
constexpr auto make_layout(Layout<Shape0, Stride0> const &layout0,
                           Layout<Shape1, Stride1> const &layout1,
                           Layouts const &...layouts) {
  return make_layout(make_layout(layout0, layout1), layouts...);
}

template <std::size_t... Is, class Shape, class Stride>
constexpr decltype(auto) shape(Layout<Shape, Stride> const &layout) {
  return layout.template shape<Is...>();
}

template <std::size_t... Is, class Shape, class Stride>
constexpr decltype(auto) stride(Layout<Shape, Stride> const &layout) {
  return layout.template stride<Is...>();
}

template <class Shape> constexpr auto make_layout(Shape const &shape) {
  return make_layout(shape, compact_col_major(shape));
}

template <class Shape>
constexpr auto make_layout(Shape const &shape, LayoutLeft) {
  return make_layout(shape, compact_col_major(shape));
}

template <class Shape>
constexpr auto make_layout(Shape const &shape, LayoutRight) {
  return make_layout(shape, compact_row_major(shape));
}

template <class Index, class Shape, class Stride>
constexpr auto idx2crd(Index const &idx, Shape const &shape,
                       Stride const &stride);

template <class Index, class Shape>
constexpr auto idx2crd(Index const &idx, Shape const &shape);

template <class Coord, class Shape, class Stride>
constexpr auto crd2idx(Coord const &coord, Shape const &shape,
                       Stride const &stride);

namespace detail {
template <class Coord, class Shape, class Stride, std::size_t I0,
          std::size_t... Is>
constexpr auto crd2idx_itt(Coord const &coord, Shape const &shape,
                           Stride const &stride,
                           std::index_sequence<I0, Is...>) {
  if constexpr (sizeof...(Is) == 0) {
    // A one-mode tuple is still an unbounded coordinate conversion.  Do not
    // introduce an extra modulo here; this matches CuTe's crd2idx contract.
    return crd2idx(coord, get<I0>(shape), get<I0>(stride));
  } else {
    auto mode_size = product(get<I0>(shape));
    auto div = coord / mode_size;
    auto mod = coord % mode_size;
    return crd2idx(mod, get<I0>(shape), get<I0>(stride)) +
           crd2idx_itt(div, shape, stride,
                       std::index_sequence<Is...>{});
  }
}

template <class Coord, class Shape, class Stride, std::size_t... Is>
constexpr auto crd2idx_ttt(Coord const &coord, Shape const &shape,
                           Stride const &stride, std::index_sequence<Is...>) {
  return sum(
      make_coord(crd2idx(get<Is>(coord), get<Is>(shape), get<Is>(stride))...));
}
} // namespace detail

template <class Coord, class Shape, class Stride>
constexpr auto crd2idx(Coord const &coord, Shape const &shape,
                       Stride const &stride) {
  if constexpr (is_tuple_v<Coord>) {
    if constexpr (is_tuple_v<Shape>) {
      if constexpr (is_tuple_v<Stride>) {
        static_assert(tuple_size_v<Coord> == tuple_size_v<Shape>,
                      "Mismatched ranks");
        static_assert(tuple_size_v<Coord> == tuple_size_v<Stride>,
                      "Mismatched ranks");
        return detail::crd2idx_ttt(
            coord, shape, stride,
            std::make_index_sequence<tuple_size_v<Coord>>{});
      } else {
        static_assert(detail::always_false_v<Coord, Shape, Stride>,
                      "Invalid parameters");
      }
    } else {
      static_assert(detail::always_false_v<Coord, Shape, Stride>,
                    "Invalid parameters");
    }
  } else {
    if constexpr (is_tuple_v<Shape>) {
      if constexpr (is_tuple_v<Stride>) {
        static_assert(tuple_size_v<Shape> == tuple_size_v<Stride>,
                      "Mismatched ranks");
        return detail::crd2idx_itt(
            coord, shape, stride,
            std::make_index_sequence<tuple_size_v<Shape>>{});
      } else {
        static_assert(detail::always_false_v<Coord, Shape, Stride>,
                      "Invalid parameters");
      }
    } else {
      if constexpr (is_tuple_v<Stride>) {
        static_assert(detail::always_false_v<Coord, Shape, Stride>,
                      "Invalid parameters");
      } else {
        return coord * stride;
      }
    }
  }
}

namespace detail {
template <class T> struct is_static_one : std::false_type {};

template <auto V> struct is_static_one<C<V>> : std::bool_constant<V == 1> {};

template <class T>
inline constexpr bool is_static_one_v = is_static_one<remove_cvref_t<T>>::value;

template <class Index, class Shape, class Stride, std::size_t... Is>
constexpr auto idx2crd_ttt(Index const &idx, Shape const &shape,
                           Stride const &stride, std::index_sequence<Is...>) {
  return make_coord(idx2crd(get<Is>(idx), get<Is>(shape), get<Is>(stride))...);
}

template <class Index, class Shape, class Stride, std::size_t... Is>
constexpr auto idx2crd_itt(Index const &idx, Shape const &shape,
                           Stride const &stride, std::index_sequence<Is...>) {
  return make_coord(idx2crd(idx, get<Is>(shape), get<Is>(stride))...);
}

template <class Index, class Shape, class Stride, std::size_t... Is>
constexpr auto idx2crd_iti(Index const &idx, Shape const &shape,
                           Stride const &stride, std::index_sequence<Is...>) {
  auto compact_stride = compact_col_major(shape, stride);
  return make_coord(idx2crd(idx, get<Is>(shape), get<Is>(compact_stride))...);
}

template <class Index, class Shape, std::size_t... Is>
constexpr auto idx2crd_tt(Index const &idx, Shape const &shape,
                          std::index_sequence<Is...>) {
  return make_coord(idx2crd(get<Is>(idx), get<Is>(shape))...);
}
} // namespace detail

template <class Index, class Shape, class Stride>
constexpr auto idx2crd(Index const &idx, Shape const &shape,
                       Stride const &stride) {
  if constexpr (is_tuple_v<Index>) {
    if constexpr (is_tuple_v<Shape>) {
      if constexpr (is_tuple_v<Stride>) {
        static_assert(tuple_size_v<Index> == tuple_size_v<Shape>,
                      "Mismatched ranks");
        static_assert(tuple_size_v<Index> == tuple_size_v<Stride>,
                      "Mismatched ranks");
        return detail::idx2crd_ttt(
            idx, shape, stride,
            std::make_index_sequence<tuple_size_v<Index>>{});
      } else {
        static_assert(detail::always_false_v<Index, Shape, Stride>,
                      "Invalid parameters");
      }
    } else {
      static_assert(detail::always_false_v<Index, Shape, Stride>,
                    "Invalid parameters");
    }
  } else {
    if constexpr (is_tuple_v<Shape>) {
      if constexpr (is_tuple_v<Stride>) {
        static_assert(tuple_size_v<Shape> == tuple_size_v<Stride>,
                      "Mismatched ranks");
        return detail::idx2crd_itt(
            idx, shape, stride,
            std::make_index_sequence<tuple_size_v<Shape>>{});
      } else {
        return detail::idx2crd_iti(
            idx, shape, stride,
            std::make_index_sequence<tuple_size_v<Shape>>{});
      }
    } else {
      if constexpr (is_tuple_v<Stride>) {
        static_assert(detail::always_false_v<Index, Shape, Stride>,
                      "Invalid parameters");
      } else if constexpr (detail::is_static_one_v<Shape>) {
        return _0{};
      } else {
        return (idx / stride) % shape;
      }
    }
  }
}

template <class Index, class Shape>
constexpr auto idx2crd(Index const &idx, Shape const &shape) {
  if constexpr (is_tuple_v<Index>) {
    if constexpr (is_tuple_v<Shape>) {
      static_assert(tuple_size_v<Index> == tuple_size_v<Shape>,
                    "Mismatched ranks");
      return detail::idx2crd_tt(
          idx, shape, std::make_index_sequence<tuple_size_v<Index>>{});
    } else {
      static_assert(detail::always_false_v<Index, Shape>, "Invalid parameters");
    }
  } else {
    if constexpr (is_tuple_v<Shape>) {
      return idx2crd(idx, shape, compact_col_major(shape));
    } else {
      return idx;
    }
  }
}

namespace detail {
template <class Coord, class Shape>
constexpr auto make_coord_like_shape(Coord const &coord, Shape const &shape);

template <class Coord, class Shape, std::size_t... Is>
constexpr auto make_coord_like_shape_impl(Coord const &coord,
                                          Shape const &shape,
                                          std::index_sequence<Is...>) {
  return make_coord(make_coord_like_shape(get<Is>(coord), get<Is>(shape))...);
}

template <class Coord, class Shape>
constexpr auto make_coord_like_shape(Coord const &coord, Shape const &shape) {
  if constexpr (is_tuple_v<Coord>) {
    if constexpr (is_tuple_v<Shape>) {
      static_assert(tuple_size_v<Coord> == tuple_size_v<Shape>,
                    "Mismatched ranks");
      return make_coord_like_shape_impl(
          coord, shape, std::make_index_sequence<tuple_size_v<Coord>>{});
    } else {
      static_assert(always_false_v<Coord, Shape>, "Invalid parameters");
    }
  } else {
    if constexpr (is_tuple_v<Shape>) {
      return idx2crd(coord, shape);
    } else {
      return coord;
    }
  }
}
} // namespace detail

template <class Coord, class Shape>
constexpr auto crd2idx(Coord const &coord, Shape const &shape) {
  auto coord_like_shape = detail::make_coord_like_shape(coord, shape);
  return crd2idx(coord_like_shape, shape, compact_col_major(shape));
}

template <class Coord, class Shape, class Stride>
constexpr auto slice(Coord const &coord, Layout<Shape, Stride> const &layout);

namespace detail {
template <class T>
inline constexpr bool is_underscore_v =
    std::is_same_v<remove_cvref_t<T>, underscore_t>;

template <class ShapePieces, class StridePieces, class Offset>
struct SliceParts {
  ShapePieces shape;
  StridePieces stride;
  Offset offset;
};

template <class ShapePieces, class StridePieces, class Offset>
constexpr auto make_slice_parts(ShapePieces const &shape,
                                StridePieces const &stride,
                                Offset const &offset) {
  return SliceParts<remove_cvref_t<ShapePieces>, remove_cvref_t<StridePieces>,
                    remove_cvref_t<Offset>>{shape, stride, offset};
}

template <class Coord, class Shape, class Stride>
constexpr auto slice_parts_node(Coord const &coord, Shape const &shape,
                                Stride const &stride);

template <class Coord, class Shape, class Stride, std::size_t... Is>
constexpr auto slice_parts_collect(Coord const &coord, Shape const &shape,
                                   Stride const &stride,
                                   std::index_sequence<Is...>) {
  return make_slice_parts(
      minicute::tuple_cat(
          slice_parts_node(get<Is>(coord), get<Is>(shape), get<Is>(stride))
              .shape...),
      minicute::tuple_cat(
          slice_parts_node(get<Is>(coord), get<Is>(shape), get<Is>(stride))
              .stride...),
      sum(make_coord(
          slice_parts_node(get<Is>(coord), get<Is>(shape), get<Is>(stride))
              .offset...)));
}

template <class Coord, class Shape, class Stride>
constexpr auto slice_parts_node(Coord const &coord, Shape const &shape,
                                Stride const &stride) {
  if constexpr (is_underscore_v<Coord>) {
    return make_slice_parts(wrap(shape), wrap(stride), _0{});
  } else if constexpr (is_tuple_v<Coord>) {
    if constexpr (is_tuple_v<Shape> && is_tuple_v<Stride>) {
      static_assert(tuple_size_v<Coord> == tuple_size_v<Shape>,
                    "Mismatched ranks");
      static_assert(tuple_size_v<Coord> == tuple_size_v<Stride>,
                    "Mismatched ranks");
      auto child_parts =
          slice_parts_collect(coord, shape, stride,
                              std::make_index_sequence<tuple_size_v<Coord>>{});
      if constexpr (tuple_size_v<decltype(child_parts.shape)> == 0) {
        return make_slice_parts(tuple<>{}, tuple<>{}, child_parts.offset);
      } else {
        return make_slice_parts(wrap(unwrap(child_parts.shape)),
                                wrap(unwrap(child_parts.stride)),
                                child_parts.offset);
      }
    } else {
      static_assert(always_false_v<Coord, Shape, Stride>, "Invalid parameters");
    }
  } else {
    return make_slice_parts(tuple<>{}, tuple<>{},
                            crd2idx(coord, shape, stride));
  }
}

template <class Coord, class Shape, class Stride>
constexpr auto slice_parts_root(Coord const &coord, Shape const &shape,
                                Stride const &stride) {
  if constexpr (is_tuple_v<Coord>) {
    if constexpr (is_tuple_v<Shape> && is_tuple_v<Stride>) {
      static_assert(tuple_size_v<Coord> == tuple_size_v<Shape>,
                    "Mismatched ranks");
      static_assert(tuple_size_v<Coord> == tuple_size_v<Stride>,
                    "Mismatched ranks");
      return slice_parts_collect(
          coord, shape, stride,
          std::make_index_sequence<tuple_size_v<Coord>>{});
    } else {
      static_assert(always_false_v<Coord, Shape, Stride>, "Invalid parameters");
    }
  } else {
    return slice_parts_node(coord, shape, stride);
  }
}
} // namespace detail

template <class Coord, class Shape, class Stride>
constexpr auto slice(Coord const &coord, Layout<Shape, Stride> const &layout) {
  auto parts = detail::slice_parts_root(coord, layout.shape(), layout.stride());
  return make_layout(unwrap(parts.shape), unwrap(parts.stride));
}

template <class Coord, class Shape, class Stride>
constexpr auto slice_and_offset(Coord const &coord,
                                Layout<Shape, Stride> const &layout) {
  auto parts = detail::slice_parts_root(coord, layout.shape(), layout.stride());
  return std::pair{make_layout(unwrap(parts.shape), unwrap(parts.stride)),
                   parts.offset};
}

template <class Shape, class Stride>
constexpr auto flatten(Layout<Shape, Stride> const &layout) {
  return make_layout(unwrap(flatten(layout.shape())),
                     unwrap(flatten(layout.stride())));
}

namespace detail {
template <class T> struct is_static_true : std::false_type {};

template <auto V>
struct is_static_true<C<V>> : std::bool_constant<static_cast<bool>(V)> {};

template <class T>
inline constexpr bool is_static_true_v =
    is_static_true<remove_cvref_t<T>>::value;

template <int I, class OldShape, class OldStride, class NewShape,
          class NewStride>
constexpr auto
coalesce_impl(OldShape const &old_shape, OldStride const &old_stride,
              NewShape const &new_shape, NewStride const &new_stride) {
  if constexpr (I == -1) {
    if constexpr (is_static_one_v<NewShape>) {
      return make_layout(_1{}, _0{});
    } else {
      return make_layout(new_shape, new_stride);
    }
  } else if constexpr (is_static_one_v<decltype(get<I>(old_shape))>) {
    return coalesce_impl<I - 1>(old_shape, old_stride, new_shape, new_stride);
  } else if constexpr (is_static_one_v<NewShape>) {
    return coalesce_impl<I - 1>(old_shape, old_stride, get<I>(old_shape),
                                get<I>(old_stride));
  } else if constexpr (is_static_integral_v<decltype(get<0>(
                           wrap(new_shape)))> &&
                       is_static_true_v<decltype(get<I>(old_shape) *
                                                     get<I>(old_stride) ==
                                                 get<0>(wrap(new_stride)))>) {
    auto merged_shape = replace<0>(wrap(new_shape),
                                   get<I>(old_shape) * get<0>(wrap(new_shape)));
    auto merged_stride = replace<0>(wrap(new_stride), get<I>(old_stride));
    return coalesce_impl<I - 1>(old_shape, old_stride, unwrap(merged_shape),
                                unwrap(merged_stride));
  } else {
    auto prepended_shape = prepend(wrap(new_shape), get<I>(old_shape));
    auto prepended_stride = prepend(wrap(new_stride), get<I>(old_stride));
    return coalesce_impl<I - 1>(old_shape, old_stride, unwrap(prepended_shape),
                                unwrap(prepended_stride));
  }
}
} // namespace detail

template <class Shape, class Stride>
constexpr auto coalesce(Layout<Shape, Stride> const &layout) {
  auto flat_shape = flatten(layout.shape());
  auto flat_stride = flatten(layout.stride());
  constexpr int R = static_cast<int>(tuple_size_v<decltype(flat_shape)>);
  if constexpr (R == 0) {
    return make_layout(_1{}, _0{});
  } else {
    return detail::coalesce_impl<R - 2>(flat_shape, flat_stride,
                                        get<R - 1>(flat_shape),
                                        get<R - 1>(flat_stride));
  }
}

template <class Shape, class Stride>
constexpr auto rank(Layout<Shape, Stride> const &layout) {
  return rank(layout.shape());
}

template <class Shape, class Stride>
constexpr auto size(Layout<Shape, Stride> const &layout) {
  return size(layout.shape());
}

namespace detail {
template <class T> constexpr auto minus_one_leaf(T const &value) {
  if constexpr (is_tuple_v<T>) {
    return transform(value, [](auto const &x) { return minus_one_leaf(x); });
  } else {
    return value - _1{};
  }
}

template <class T> constexpr auto abs_leaf(T const &value) {
  if constexpr (is_tuple_v<T>) {
    return transform(value, [](auto const &x) { return abs_leaf(x); });
  } else if constexpr (is_static_integral_v<T>) {
    constexpr auto v = remove_cvref_t<T>::value;
    return C<(v < 0 ? -v : v)>{};
  } else {
    return value < 0 ? -value : value;
  }
}
} // namespace detail

template <class Shape, class Stride>
constexpr auto cosize(Layout<Shape, Stride> const &layout) {
  return inner_product(detail::minus_one_leaf(layout.shape()),
                       detail::abs_leaf(layout.stride())) +
         _1{};
}

namespace detail {
template <class Stride, class Shape>
constexpr auto filter_zeros_value(Stride const &stride, Shape const &shape) {
  if constexpr (is_tuple_v<Stride>) {
    static_assert(is_tuple_v<Shape>,
                  "filter_zeros requires matching Shape and Stride profiles");
    static_assert(tuple_size_v<Stride> == tuple_size_v<Shape>,
                  "filter_zeros requires matching Shape and Stride ranks");

    return transform(stride, shape, [](auto const &s, auto const &sh) {
      return filter_zeros_value(s, sh);
    });
  } else if constexpr (is_tuple_v<Shape>) {
    static_assert(always_false_v<Stride, Shape>,
                  "filter_zeros requires matching Shape and Stride profiles");
  } else if constexpr (is_constant_v<0, Stride>) {
    return _1{};
  } else {
    return shape;
  }
}
} // namespace detail

template <class Shape, class Stride>
constexpr auto filter_zeros(Layout<Shape, Stride> const &layout) {
  return make_layout(
      detail::filter_zeros_value(layout.stride(), layout.shape()),
      layout.stride());
}

template <class Shape, class Stride>
constexpr auto filter(Layout<Shape, Stride> const &layout) {
  return coalesce(filter_zeros(layout));
}

// Compose two ordinary Layouts and reduce the result to another ordinary
// Layout whenever the mode-wise divisibility conditions permit it.  The
// implementation follows the mixed-radix decomposition used by CuTe, but
// keeps the scan state local to this function so the result types remain
// visible to C++20's constexpr deduction.
template <class LShape, class LStride, class RShape, class RStride>
constexpr auto composition(Layout<LShape, LStride> const &lhs,
                           Layout<RShape, RStride> const &rhs) {
  static_assert(is_tuple_v<LShape> == is_tuple_v<LStride>,
                "composition requires matching LHS Shape and Stride profiles");
  static_assert(is_tuple_v<RShape> == is_tuple_v<RStride>,
                "composition requires matching RHS Shape and Stride profiles");

  if constexpr (is_tuple_v<LShape>) {
    static_assert(tuple_size_v<LShape> == tuple_size_v<LStride>,
                  "composition requires matching LHS Shape and Stride ranks");
  }
  if constexpr (is_tuple_v<RShape>) {
    static_assert(tuple_size_v<RShape> == tuple_size_v<RStride>,
                  "composition requires matching RHS Shape and Stride ranks");
  }

  // The scan returns (result_shape, result_stride, rest_shape, rest_stride).
  // The first two fields are already emitted modes; the latter two describe
  // the part of the RHS scalar mode that has not been consumed yet.
  auto scan = [&]<int I, class ScanLShape, class ScanLStride,
                class ResultShape, class ResultStride, class RestShape,
                class RestStride>(
                    auto &&self, ScanLShape const &lhs_shape,
                    ScanLStride const &lhs_stride,
                    ResultShape const &result_shape,
                    ResultStride const &result_stride,
                    RestShape const &rest_shape,
                    RestStride const &rest_stride) -> auto {
    constexpr int lhs_rank = static_cast<int>(tuple_size_v<ScanLShape>);

    if constexpr (I >= lhs_rank - 1) {
      return make_tuple(result_shape, result_stride, rest_shape,
                        rest_stride);
    } else {
      auto curr_shape = get<I>(lhs_shape);
      auto curr_stride = get<I>(lhs_stride);

      if constexpr (is_static_integral_v<decltype(curr_shape)> &&
                    is_static_integral_v<decltype(rest_stride)>) {
        using CurrShape = remove_cvref_t<decltype(curr_shape)>;
        using RestStrideValue = remove_cvref_t<decltype(rest_stride)>;
        constexpr auto curr_shape_value = CurrShape::value;
        constexpr auto rest_stride_value = RestStrideValue::value;
        constexpr auto rest_stride_abs =
            detail::static_abs(rest_stride_value);
        static_assert((rest_stride_abs % curr_shape_value == 0) ||
                          (rest_stride_abs < curr_shape_value),
                      "composition stride divisibility condition failed");
      }

      auto next_shape = ceil_div(curr_shape, abs(rest_stride));
      auto next_stride =
          ceil_div(abs(rest_stride), curr_shape) * signum(rest_stride);

      if constexpr (is_constant_v<1, decltype(next_shape)> ||
                    is_constant_v<1, decltype(rest_shape)>) {
        // The current mode contributes no independent result mode.  The
        // remaining RHS shape is unchanged, but its stride is now measured
        // relative to the next LHS mode.
        return self.template operator()<I + 1>(
            self, lhs_shape, lhs_stride, result_shape, result_stride,
            rest_shape, next_stride);
      } else {
        auto new_shape = min(next_shape, rest_shape);

        auto new_result_shape = append(result_shape, new_shape);
        auto new_result_stride =
            append(result_stride, rest_stride * curr_stride);

        if constexpr (is_static_integral_v<decltype(new_shape)> &&
                      is_static_integral_v<decltype(rest_shape)>) {
          using NewShape = remove_cvref_t<decltype(new_shape)>;
          using RestShapeValue = remove_cvref_t<decltype(rest_shape)>;
          constexpr auto new_shape_value = NewShape::value;
          constexpr auto rest_shape_value = RestShapeValue::value;
          static_assert((rest_shape_value % new_shape_value) == 0,
                        "composition shape divisibility condition failed");
        }

        return self.template operator()<I + 1>(
            self, lhs_shape, lhs_stride, new_result_shape,
            new_result_stride, rest_shape / new_shape, next_stride);
      }
    }
  };

  // RHS tuple modes are independent modes of the composed layout.  Compose
  // each pair recursively, then concatenate the resulting ordinary Layouts
  // rather than returning tuple<Layout...>.
  auto compose_rhs_modes = [&]<class Self, class TupleLShape,
                              class TupleLStride, class TupleRShape,
                              class TupleRStride, std::size_t... Is>(
                                 Self &&self, TupleLShape const &lhs_shape,
                                 TupleLStride const &lhs_stride,
                                 TupleRShape const &rhs_shape,
                                 TupleRStride const &rhs_stride,
                                 std::index_sequence<Is...>) -> auto {
    if constexpr (sizeof...(Is) == 0) {
      return make_layout(make_shape(), make_stride());
    } else {
      return make_layout(
          self(self, lhs_shape, lhs_stride, get<Is>(rhs_shape),
               get<Is>(rhs_stride))...);
    }
  };

  // Composition needs the outside-domain-preserving variant of coalesce.
  // In particular, (1):(1) must not become (1):(0) when the RHS domain is
  // larger than one element.
  auto coalesce_for_composition = [&]<class Shape, class Stride>(
                                      Layout<Shape, Stride> const &layout) {
    auto flat_shape = flatten(layout.shape());
    auto flat_stride = flatten(layout.stride());
    constexpr int flat_rank =
        static_cast<int>(tuple_size_v<decltype(flat_shape)>);

    if constexpr (flat_rank == 0) {
      return make_layout(_1{}, _0{});
    } else if constexpr (detail::is_static_one_v<decltype(
                           get<flat_rank - 1>(flat_shape))>) {
      return detail::coalesce_impl<flat_rank - 2>(
          flat_shape, flat_stride, _2{}, get<flat_rank - 1>(flat_stride));
    } else {
      return detail::coalesce_impl<flat_rank - 2>(
          flat_shape, flat_stride, get<flat_rank - 1>(flat_shape),
          get<flat_rank - 1>(flat_stride));
    }
  };

  auto compose_one = [&]<class Self, class ComposeLShape,
                        class ComposeLStride, class ComposeRShape,
                        class ComposeRStride>(
                         Self &&self, ComposeLShape const &lhs_shape,
                         ComposeLStride const &lhs_stride,
                         ComposeRShape const &rhs_shape,
                         ComposeRStride const &rhs_stride) -> auto {
    if constexpr (is_tuple_v<ComposeRShape>) {
      return compose_rhs_modes(
          self, lhs_shape, lhs_stride, rhs_shape, rhs_stride,
          std::make_index_sequence<tuple_size_v<ComposeRShape>>{});
    } else if constexpr (is_tuple_v<ComposeRStride>) {
      static_assert(detail::always_false_v<ComposeRShape, ComposeRStride>,
                    "composition requires matching RHS Shape and Stride");
    } else if constexpr (is_constant_v<0, ComposeRStride>) {
      // A static stride-0 RHS always feeds the same input coordinate to the
      // LHS, so its shape and stride already describe the result domain.
      return make_layout(rhs_shape, rhs_stride);
    } else if constexpr (!is_tuple_v<ComposeLShape>) {
      static_assert(!is_tuple_v<ComposeLStride>,
                    "composition requires matching LHS Shape and Stride");
      return make_layout(rhs_shape, rhs_stride * lhs_stride);
    } else {
      static_assert(is_tuple_v<ComposeLStride>,
                    "composition requires matching LHS Shape and Stride");
      constexpr int lhs_rank = static_cast<int>(tuple_size_v<ComposeLShape>);
      static_assert(lhs_rank > 0,
                    "composition requires a non-empty LHS shape");

      auto flat_lhs = coalesce_for_composition(
          make_layout(lhs_shape, lhs_stride));
      auto flat_shape = flat_lhs.shape();
      auto flat_stride = flat_lhs.stride();

      if constexpr (!is_tuple_v<decltype(flat_shape)>) {
        return make_layout(rhs_shape, rhs_stride * flat_stride);
      } else {
        auto state = scan.template operator()<0>(
            scan, flat_shape, flat_stride, tuple<>{}, tuple<>{}, rhs_shape,
            rhs_stride);

        auto result_shape = get<0>(state);
        auto result_stride = get<1>(state);
        auto rest_shape = get<2>(state);
        auto rest_stride = get<3>(state);
        auto last_stride =
            get<tuple_size_v<decltype(flat_stride)> - 1>(flat_stride);

        auto final_stride = rest_stride * last_stride;

        if constexpr (tuple_size_v<decltype(result_shape)> == 0) {
          return make_layout(rest_shape, final_stride);
        } else if constexpr (is_constant_v<1, decltype(rest_shape)>) {
          return make_layout(unwrap(result_shape), unwrap(result_stride));
        } else {
          return make_layout(append(result_shape, rest_shape),
                             append(result_stride, final_stride));
        }
      }
    }
  };

  return compose_one(compose_one, lhs.shape(), lhs.stride(), rhs.shape(),
                     rhs.stride());
}
} // namespace minicute
