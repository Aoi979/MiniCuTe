#pragma once

#include <minicute/tensor.hpp>

#include <ostream>

namespace minicute {

namespace detail {

template <class Shape, bool = is_tuple_v<Shape>>
struct shape_rank : std::integral_constant<int, 1> {};

template <class Shape>
struct shape_rank<Shape, true>
    : std::integral_constant<int,
                             static_cast<int>(tuple_size_v<remove_cvref_t<Shape>>)> {};

template <class T>
struct layout_rank : std::integral_constant<int, 1> {};

template <class Shape, class Stride>
struct layout_rank<Layout<Shape, Stride>> : shape_rank<Shape> {};

}  // namespace detail

template <class LayoutA, class Offset, class LayoutB>
struct ComposedLayout : private tuple<LayoutA, Offset, LayoutB> {
  static constexpr int rank = detail::layout_rank<remove_cvref_t<LayoutB>>::value;

  using base_type = tuple<LayoutA, Offset, LayoutB>;

  constexpr ComposedLayout(LayoutA const& layout_a = {},
                           Offset const& offset = {},
                           LayoutB const& layout_b = {})
      : base_type(layout_a, offset, layout_b) {}

  constexpr decltype(auto) layout_a() const {
    return get<0>(static_cast<base_type const&>(*this));
  }

  constexpr decltype(auto) offset() const {
    return get<1>(static_cast<base_type const&>(*this));
  }

  constexpr decltype(auto) layout_b() const {
    return get<2>(static_cast<base_type const&>(*this));
  }

  constexpr decltype(auto) layout() const { return (*this); }

  template <std::size_t... Is>
  constexpr decltype(auto) shape() const {
    if constexpr (sizeof...(Is) == 0) {
      return layout_b().shape();
    } else {
      return get<Is...>(layout_b().shape());
    }
  }


  template <std::size_t... Is>
  constexpr decltype(auto) stride() const = delete;

  template <class Coord>
  constexpr auto operator()(Coord const& coord) const {
    static_assert(
        requires(LayoutA const& layout_a, Offset const& offset,
                 LayoutB const& layout_b, Coord const& c) {
          layout_a(offset + layout_b(c));
        },
        "ComposedLayout requires LayoutA(offset + LayoutB(coord)) to be valid");
    return layout_a()(offset() + layout_b()(coord));
  }

  template <class Coord0, class Coord1, class... Coords>
  constexpr auto operator()(Coord0 const& c0, Coord1 const& c1,
                            Coords const&... cs) const {
    return (*this)(make_coord(c0, c1, cs...));
  }

};

template <class LayoutA, class Offset, class LayoutB>
struct detail::layout_rank<ComposedLayout<LayoutA, Offset, LayoutB>>
    : std::integral_constant<int,
                             ComposedLayout<LayoutA, Offset, LayoutB>::rank> {};

template <class LayoutA, class Offset, class LayoutB>
struct is_layout<ComposedLayout<LayoutA, Offset, LayoutB>> : true_type {};

template <class T>
struct is_composed_layout : false_type {};

template <class LayoutA, class Offset, class LayoutB>
struct is_composed_layout<ComposedLayout<LayoutA, Offset, LayoutB>>
    : true_type {};

template <class T>
inline constexpr bool is_composed_layout_v =
    is_composed_layout<remove_cvref_t<T>>::value;

template <class LayoutA, class Offset, class LayoutB>
constexpr auto make_composed_layout(LayoutA const& layout_a,
                                    Offset const& offset,
                                    LayoutB const& layout_b) {
  return ComposedLayout<remove_cvref_t<LayoutA>, remove_cvref_t<Offset>,
                        remove_cvref_t<LayoutB>>{layout_a, offset, layout_b};
}

template <std::size_t... Is, class Shape, class Stride>
constexpr decltype(auto) layout(Layout<Shape, Stride> const& layout_value) {
  if constexpr (sizeof...(Is) == 0) {
    return (layout_value);
  } else {
    return make_layout(get<Is...>(layout_value.shape()),
                       get<Is...>(layout_value.stride()));
  }
}

template <std::size_t... Is, class LayoutA, class Offset, class LayoutB>
constexpr decltype(auto) layout(
    ComposedLayout<LayoutA, Offset, LayoutB> const& composed) {
  if constexpr (sizeof...(Is) == 0) {
    return composed;
  } else {
    return composition(composed.layout_a(), composed.offset(),
                       layout<Is...>(composed.layout_b()));
  }
}

template <std::size_t... Is, class LayoutA, class Offset, class LayoutB>
constexpr decltype(auto) shape(
    ComposedLayout<LayoutA, Offset, LayoutB> const& composed) {
  return composed.template shape<Is...>();
}

template <std::size_t... Is, class LayoutA, class Offset, class LayoutB>
constexpr decltype(auto) stride(
    ComposedLayout<LayoutA, Offset, LayoutB> const&) = delete;

template <std::size_t... Is, class LayoutA, class Offset, class LayoutB>
constexpr decltype(auto) size(
    ComposedLayout<LayoutA, Offset, LayoutB> const& composed) {
  return size(composed.template shape<Is...>());
}

template <std::size_t... Is, class LayoutA, class Offset, class LayoutB>
constexpr auto rank(
    ComposedLayout<LayoutA, Offset, LayoutB> const& composed) {
  return rank(composed.template shape<Is...>());
}

template <std::size_t... Is, class LayoutA, class Offset, class LayoutB>
constexpr auto depth(
    ComposedLayout<LayoutA, Offset, LayoutB> const& composed) {
  return depth(composed.template shape<Is...>());
}

template <std::size_t... Is, class LayoutA, class Offset, class LayoutB>
constexpr auto cosize(
    ComposedLayout<LayoutA, Offset, LayoutB> const& composed) {
  return cosize(composed.layout_b());
}

template <class LayoutA, class Offset, class LayoutB>
constexpr auto composition(LayoutA const& layout_a, Offset const& offset,
                           LayoutB const& layout_b) {
  return make_composed_layout(layout_a, offset, layout_b);
}


template <class Lhs, class Rhs>
constexpr auto composition(Lhs const& lhs, Rhs const& rhs) {
  return make_composed_layout(lhs, _0{}, rhs);
}

template <class LayoutA, class Offset, class LayoutB, class Other>
constexpr auto composition(
    ComposedLayout<LayoutA, Offset, LayoutB> const& lhs,
    Other const& rhs) {
  return composition(lhs.layout_a(), lhs.offset(),
                     composition(lhs.layout_b(), rhs));
}

template <class LShape, class LStride, class LayoutA, class Offset,
          class LayoutB>
constexpr auto composition(
    Layout<LShape, LStride> const& lhs,
    ComposedLayout<LayoutA, Offset, LayoutB> const& rhs) {
  static_assert(is_constant_v<0, Offset>,
                "composition(Layout, ComposedLayout) requires zero offset");
  return composition(composition(lhs, rhs.layout_a()), rhs.layout_b());
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto composition(Tensor<Engine, TensorLayout>& tensor,
                            Tiler const& tiler) {
  return detail::rebind_tensor(tensor, composition(tensor.layout(), tiler));
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto composition(Tensor<Engine, TensorLayout> const& tensor,
                            Tiler const& tiler) {
  return detail::rebind_tensor(tensor, composition(tensor.layout(), tiler));
}

template <class Coord, class SrcShape, class DstShape>
constexpr auto crd2crd(Coord const& coord, SrcShape const& src_shape,
                       DstShape const& dst_shape) {
  return idx2crd(crd2idx(coord, src_shape), dst_shape);
}

namespace detail {
template <class Coord, class Value>
constexpr auto dice_value(Coord const& coord, Value const& value);

template <class Coord, class Value, std::size_t... Is>
constexpr auto dice_tuple(Coord const& coord, Value const& value,
                          std::index_sequence<Is...>) {
  return minicute::tuple_cat(dice_value(get<Is>(coord), get<Is>(value))...);
}

template <class Coord, class Value>
constexpr auto dice_value(Coord const& coord, Value const& value) {
  if constexpr (is_underscore_v<Coord>) {
    return tuple<>{};
  } else if constexpr (is_tuple_v<Coord>) {
    if constexpr (is_tuple_v<Value>) {
      static_assert(tuple_size_v<Coord> == tuple_size_v<Value>,
                    "Mismatched ranks");
      auto children =
          dice_tuple(coord, value, std::make_index_sequence<tuple_size_v<Coord>>{});
      if constexpr (tuple_size_v<decltype(children)> == 0) {
        return tuple<>{};
      } else {
        return wrap(unwrap(children));
      }
    } else {
      static_assert(always_false_v<Coord, Value>, "Invalid parameters");
    }
  } else {
    return wrap(value);
  }
}
} // namespace detail

template <class Coord, class Shape, class Stride>
constexpr auto dice(Coord const& coord, Layout<Shape, Stride> const& layout) {
  return make_layout(unwrap(detail::dice_value(coord, layout.shape())),
                     unwrap(detail::dice_value(coord, layout.stride())));
}

template <class Coord, class Shape, class Stride>
constexpr auto domain_offset(Coord const& coord,
                             Layout<Shape, Stride> const& layout) {
  return slice_and_offset(coord, layout);
}

template <class LayoutLike>
constexpr auto right_inverse(LayoutLike const&) {
  static_assert(detail::always_false_v<LayoutLike>,
                "TODO(stage4): implement right_inverse");
}

template <class LayoutLike>
constexpr auto left_inverse(LayoutLike const&) {
  static_assert(detail::always_false_v<LayoutLike>,
                "TODO(stage4): implement left_inverse");
}

namespace detail {
template <class T>
std::ostream& print_value(std::ostream& os, T const& value);

template <class Tuple, std::size_t... Is>
std::ostream& print_tuple(std::ostream& os, Tuple const& tuple,
                          std::index_sequence<Is...>) {
  os << "(";
  ((os << (Is == 0 ? "" : ", "), print_value(os, get<Is>(tuple))), ...);
  return os << ")";
}

template <class T>
std::ostream& print_value(std::ostream& os, T const& value) {
  if constexpr (is_tuple_v<T>) {
    return print_tuple(os, value, std::make_index_sequence<tuple_size_v<T>>{});
  } else {
    return os << value;
  }
}
} // namespace detail

template <class Engine, class Layout>
std::ostream& print_tensor(std::ostream& os, Tensor<Engine, Layout> const& tensor) {
  os << "Tensor(shape=";
  detail::print_value(os, tensor.shape());
  os << ", data=[";

  auto n = size(tensor.layout());
  for (int i = 0; i < static_cast<int>(n); ++i) {
    if (i != 0) {
      os << ", ";
    }
    auto coord = idx2crd(i, tensor.shape());
    os << tensor(coord);
  }

  return os << "])";
}

}  // namespace minicute
