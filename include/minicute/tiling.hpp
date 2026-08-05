#pragma once

#include <minicute/layout_composed.hpp>

namespace minicute {

template <class Shape, class Order>
constexpr auto make_ordered_layout(Shape const&, Order const&) {
  static_assert(detail::always_false_v<Shape, Order>,
                "TODO(stage5): implement make_ordered_layout");
}

template <class LShape, class LStride, class TShape, class TStride>
constexpr auto logical_divide(
    Layout<LShape, LStride> const& layout,
    Layout<TShape, TStride> const& tiler) {
  auto rest = complement(tiler, shape(coalesce(layout)));
  return composition(layout, make_layout(tiler, rest));
}

template <class Shape, class Stride, class Tiler>
constexpr auto logical_divide(Layout<Shape, Stride> const& layout,
                              Tiler const& tiler) {
  if constexpr (is_tuple_v<Tiler>) {
    constexpr auto layout_rank =
        static_cast<std::size_t>(decltype(rank(layout.shape()))::value);
    static_assert(tuple_size_v<Tiler> <= layout_rank,
                  "logical_divide: too many tiler modes");
    static_assert(tuple_size_v<Tiler> != 0,
                  "logical_divide: empty tiler is not supported");

    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return [&]<std::size_t... RestIs>(
          std::index_sequence<RestIs...>) {
        return make_layout(
            logical_divide(minicute::layout<Is>(layout), get<Is>(tiler))...,
            minicute::layout<tuple_size_v<Tiler> + RestIs>(layout)...);
      }(std::make_index_sequence<layout_rank - tuple_size_v<Tiler>>{});
    }(std::make_index_sequence<tuple_size_v<Tiler>>{});
  } else if constexpr (detail::is_underscore_v<Tiler>) {
    return layout;
  } else if constexpr (is_integral_v<Tiler>) {
    return logical_divide(layout, make_layout(tiler));
  } else {
    static_assert(detail::always_false_v<Shape, Stride, Tiler>,
                  "logical_divide requires a Layout, Shape, or underscore tiler");
  }
}

template <class LayoutA, class Offset, class LayoutB, class Tiler>
constexpr auto logical_divide(ComposedLayout<LayoutA, Offset, LayoutB> const& layout,
                              Tiler const& tiler) {
  return composition(layout.layout_a(), layout.offset(),
                     logical_divide(layout.layout_b(), tiler));
}

template <class Shape, class Stride, class Tiler>
constexpr auto zipped_divide(Layout<Shape, Stride> const&, Tiler const&) {
  static_assert(detail::always_false_v<Shape, Stride, Tiler>,
                "TODO(stage5): implement zipped_divide");
}

template <class Shape, class Stride, class Tiler>
constexpr auto tiled_divide(Layout<Shape, Stride> const&, Tiler const&) {
  static_assert(detail::always_false_v<Shape, Stride, Tiler>,
                "TODO(stage5): implement tiled_divide");
}

template <class Shape, class Stride, class Tiler>
constexpr auto flat_divide(Layout<Shape, Stride> const&, Tiler const&) {
  static_assert(detail::always_false_v<Shape, Stride, Tiler>,
                "TODO(stage5): implement flat_divide");
}

template <class BlockShape, class BlockStride, class TilerShape,
          class TilerStride>
constexpr auto logical_product(
    Layout<BlockShape, BlockStride> const& block,
    Layout<TilerShape, TilerStride> const& tiler) {
  auto complement_layout =
      complement(block, size(block) * cosize(tiler));
  return make_layout(block, composition(complement_layout, tiler));
}

template <class Shape, class Stride, class Tiler>
constexpr auto blocked_product(Layout<Shape, Stride> const&, Tiler const&) {
  static_assert(detail::always_false_v<Shape, Stride, Tiler>,
                "TODO(stage5): implement blocked_product");
}

template <class Shape, class Stride, class Tiler>
constexpr auto raked_product(Layout<Shape, Stride> const&, Tiler const&) {
  static_assert(detail::always_false_v<Shape, Stride, Tiler>,
                "TODO(stage5): implement raked_product");
}

template <class Shape, class Stride, class TargetShape>
constexpr auto tile_to_shape(Layout<Shape, Stride> const&, TargetShape const&) {
  static_assert(detail::always_false_v<Shape, Stride, TargetShape>,
                "TODO(stage5): implement tile_to_shape");
}

template <class Engine, class Layout, class TileShape, class TileCoord>
constexpr auto local_tile(Tensor<Engine, Layout>&, TileShape const&,
                          TileCoord const&) {
  static_assert(detail::always_false_v<Engine, Layout, TileShape, TileCoord>,
                "TODO(stage5): implement local_tile");
}

template <class Engine, class Layout, class TileShape, class TileCoord>
constexpr auto local_tile_clamped(Tensor<Engine, Layout>&, TileShape const&,
                                  TileCoord const&) {
  static_assert(detail::always_false_v<Engine, Layout, TileShape, TileCoord>,
                "TODO(stage5): implement local_tile_clamped");
}

}  // namespace minicute
