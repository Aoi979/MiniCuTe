#pragma once

#include "tensor.hpp"

#include <ostream>

namespace minicute {

template <class Lhs, class Rhs>
struct ComposedLayout {
  constexpr ComposedLayout(Lhs const& lhs = {}, Rhs const& rhs = {})
      : lhs_(lhs), rhs_(rhs) {}

  constexpr decltype(auto) shape() const { return rhs_.shape(); }

  template <class Coord>
  constexpr auto operator()(Coord const& coord) const {
    static_assert(requires(Lhs const& lhs, Rhs const& rhs, Coord const& c) {
      lhs(rhs(c));
    }, "ComposedLayout requires rhs(coord) to be a valid coordinate for lhs");
    return lhs_(rhs_(coord));
  }

  template <class Coord0, class Coord1, class... Coords>
  constexpr auto operator()(Coord0 const& c0, Coord1 const& c1,
                            Coords const&... cs) const {
    return (*this)(make_coord(c0, c1, cs...));
  }

  Lhs lhs_;
  Rhs rhs_;
};

template <class Lhs, class Rhs>
constexpr decltype(auto) shape(ComposedLayout<Lhs, Rhs> const& layout) {
  return layout.shape();
}

template <class Lhs, class Rhs>
constexpr auto composition(Lhs const& lhs, Rhs const& rhs) {
  return ComposedLayout<remove_cvref_t<Lhs>, remove_cvref_t<Rhs>>{lhs, rhs};
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

template <class LayoutLike, class CoTarget>
constexpr auto complement(LayoutLike const&, CoTarget const&) {
  static_assert(detail::always_false_v<LayoutLike, CoTarget>,
                "TODO(stage4): implement complement(layout, cotarget)");
}

template <class LayoutLike>
constexpr auto complement(LayoutLike const&) {
  static_assert(detail::always_false_v<LayoutLike>,
                "TODO(stage4): implement complement(layout)");
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
