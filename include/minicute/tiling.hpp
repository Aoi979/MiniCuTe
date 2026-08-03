#pragma once

#include <minicute/layout_composed.hpp>

namespace minicute {

template <class Shape, class Order>
constexpr auto compact_order(Shape const&, Order const&) {
  static_assert(detail::always_false_v<Shape, Order>,
                "TODO(stage5): implement compact_order");
}

template <class Shape, class Order>
constexpr auto make_ordered_layout(Shape const&, Order const&) {
  static_assert(detail::always_false_v<Shape, Order>,
                "TODO(stage5): implement make_ordered_layout");
}

template <class Shape, class Stride, class Tiler>
constexpr auto logical_divide(Layout<Shape, Stride> const&, Tiler const&) {
  static_assert(detail::always_false_v<Shape, Stride, Tiler>,
                "TODO(stage5): implement logical_divide");
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

template <class Shape, class Stride, class Tiler>
constexpr auto logical_product(Layout<Shape, Stride> const&, Tiler const&) {
  static_assert(detail::always_false_v<Shape, Stride, Tiler>,
                "TODO(stage5): implement logical_product");
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
