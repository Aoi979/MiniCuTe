#pragma once

#include <minicute/layout_composed.hpp>

namespace minicute {

template <class Shape, class Order>
constexpr auto make_ordered_layout(Shape const& shape, Order const& order) {
  return make_layout(shape, compact_order(shape, order));
}

template <class LShape, class LStride, class TShape, class TStride>
constexpr auto logical_divide(
    Layout<LShape, LStride> const& layout,
    Layout<TShape, TStride> const& tiler) {
  auto rest = complement(tiler, shape(coalesce(layout)));
  return composition(layout, make_layout(tiler, rest));
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto logical_divide(Tensor<Engine, TensorLayout>& tensor,
                              Tiler const& tiler) {
  return detail::rebind_tensor(tensor, logical_divide(tensor.layout(), tiler));
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto logical_divide(Tensor<Engine, TensorLayout> const& tensor,
                              Tiler const& tiler) {
  return detail::rebind_tensor(tensor, logical_divide(tensor.layout(), tiler));
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

namespace detail {

template <class Result, std::size_t... Is>
constexpr auto tiled_unzip_impl(Result const& result,
                                std::index_sequence<Is...>) {
  return make_layout(minicute::layout<0>(result),
                     minicute::layout<1, Is>(result)...);
}

template <class Result, std::size_t... I0, std::size_t... I1>
constexpr auto flat_unzip_impl(Result const& result,
                               std::index_sequence<I0...>,
                               std::index_sequence<I1...>) {
  return make_layout(minicute::layout<0, I0>(result)...,
                     minicute::layout<1, I1>(result)...);
}

}  // namespace detail

template <class Shape, class Stride, class Tiler>
constexpr auto zipped_divide(Layout<Shape, Stride> const &layout,
                             Tiler const &tiler) {
  auto divided = logical_divide(layout, tiler);
  return make_layout(zip2_by(shape(divided), tiler),
                     zip2_by(stride(divided), tiler));
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto zipped_divide(Tensor<Engine, TensorLayout>& tensor,
                             Tiler const& tiler) {
  return detail::rebind_tensor(tensor, zipped_divide(tensor.layout(), tiler));
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto zipped_divide(Tensor<Engine, TensorLayout> const& tensor,
                             Tiler const& tiler) {
  return detail::rebind_tensor(tensor, zipped_divide(tensor.layout(), tiler));
}

template <class Shape, class Stride, class Tiler>
constexpr auto tiled_divide(Layout<Shape, Stride> const& layout,
                            Tiler const& tiler) {
  auto result = zipped_divide(layout, tiler);
  constexpr std::size_t result_rank = static_cast<std::size_t>(
      decltype(rank(shape<1>(result)))::value);

  return detail::tiled_unzip_impl(
      result, std::make_index_sequence<result_rank>{});
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto tiled_divide(Tensor<Engine, TensorLayout>& tensor,
                            Tiler const& tiler) {
  return detail::rebind_tensor(tensor, tiled_divide(tensor.layout(), tiler));
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto tiled_divide(Tensor<Engine, TensorLayout> const& tensor,
                            Tiler const& tiler) {
  return detail::rebind_tensor(tensor, tiled_divide(tensor.layout(), tiler));
}

template <class Shape, class Stride, class Tiler>
constexpr auto flat_divide(Layout<Shape, Stride> const& layout,
                           Tiler const& tiler) {
  auto result = zipped_divide(layout, tiler);
  constexpr std::size_t result_rank0 = static_cast<std::size_t>(
      decltype(rank(shape<0>(result)))::value);
  constexpr std::size_t result_rank1 = static_cast<std::size_t>(
      decltype(rank(shape<1>(result)))::value);

  return detail::flat_unzip_impl(
      result, std::make_index_sequence<result_rank0>{},
      std::make_index_sequence<result_rank1>{});
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto flat_divide(Tensor<Engine, TensorLayout>& tensor,
                           Tiler const& tiler) {
  return detail::rebind_tensor(tensor, flat_divide(tensor.layout(), tiler));
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto flat_divide(Tensor<Engine, TensorLayout> const& tensor,
                           Tiler const& tiler) {
  return detail::rebind_tensor(tensor, flat_divide(tensor.layout(), tiler));
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
constexpr auto logical_product(Layout<Shape, Stride> const& block,
                               Tiler const& tiler) {
  if constexpr (is_tuple_v<Tiler>) {
    constexpr auto block_rank =
        static_cast<std::size_t>(decltype(rank(block.shape()))::value);
    static_assert(tuple_size_v<Tiler> <= block_rank,
                  "logical_product: too many tiler modes");
    static_assert(tuple_size_v<Tiler> != 0,
                  "logical_product: empty tiler is not supported");

    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return [&]<std::size_t... RestIs>(
          std::index_sequence<RestIs...>) {
        return make_layout(
            logical_product(minicute::layout<Is>(block), get<Is>(tiler))...,
            minicute::layout<tuple_size_v<Tiler> + RestIs>(block)...);
      }(std::make_index_sequence<block_rank - tuple_size_v<Tiler>>{});
    }(std::make_index_sequence<tuple_size_v<Tiler>>{});
  } else if constexpr (detail::is_underscore_v<Tiler>) {
    return block;
  } else if constexpr (is_integral_v<Tiler>) {
    return logical_product(block, make_layout(tiler));
  } else {
    static_assert(detail::always_false_v<Shape, Stride, Tiler>,
                  "logical_product requires a Layout, Shape, or underscore tiler");
  }
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto logical_product(Tensor<Engine, TensorLayout>& tensor,
                               Tiler const& tiler) {
  return detail::rebind_tensor(tensor,
                               logical_product(tensor.layout(), tiler));
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto logical_product(Tensor<Engine, TensorLayout> const& tensor,
                               Tiler const& tiler) {
  return detail::rebind_tensor(tensor,
                               logical_product(tensor.layout(), tiler));
}

template <class Shape, class Stride, class Tiler>
constexpr auto zipped_product(Layout<Shape, Stride> const& block,
                              Tiler const& tiler) {
  auto result = logical_product(block, tiler);
  return make_layout(zip2_by(shape(result), tiler),
                     zip2_by(stride(result), tiler));
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto zipped_product(Tensor<Engine, TensorLayout>& tensor,
                              Tiler const& tiler) {
  return detail::rebind_tensor(tensor,
                               zipped_product(tensor.layout(), tiler));
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto zipped_product(Tensor<Engine, TensorLayout> const& tensor,
                              Tiler const& tiler) {
  return detail::rebind_tensor(tensor,
                               zipped_product(tensor.layout(), tiler));
}

template <class Shape, class Stride, class Tiler>
constexpr auto tiled_product(Layout<Shape, Stride> const& block,
                             Tiler const& tiler) {
  auto result = zipped_product(block, tiler);
  constexpr std::size_t result_rank = static_cast<std::size_t>(
      decltype(rank(shape<1>(result)))::value);

  return detail::tiled_unzip_impl(
      result, std::make_index_sequence<result_rank>{});
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto tiled_product(Tensor<Engine, TensorLayout>& tensor,
                             Tiler const& tiler) {
  return detail::rebind_tensor(tensor,
                               tiled_product(tensor.layout(), tiler));
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto tiled_product(Tensor<Engine, TensorLayout> const& tensor,
                             Tiler const& tiler) {
  return detail::rebind_tensor(tensor,
                               tiled_product(tensor.layout(), tiler));
}

template <class Shape, class Stride, class Tiler>
constexpr auto flat_product(Layout<Shape, Stride> const& block,
                            Tiler const& tiler) {
  auto result = zipped_product(block, tiler);
  constexpr std::size_t result_rank0 = static_cast<std::size_t>(
      decltype(rank(shape<0>(result)))::value);
  constexpr std::size_t result_rank1 = static_cast<std::size_t>(
      decltype(rank(shape<1>(result)))::value);

  return detail::flat_unzip_impl(
      result, std::make_index_sequence<result_rank0>{},
      std::make_index_sequence<result_rank1>{});
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto flat_product(Tensor<Engine, TensorLayout>& tensor,
                            Tiler const& tiler) {
  return detail::rebind_tensor(tensor,
                               flat_product(tensor.layout(), tiler));
}

template <class Engine, class TensorLayout, class Tiler>
constexpr auto flat_product(Tensor<Engine, TensorLayout> const& tensor,
                            Tiler const& tiler) {
  return detail::rebind_tensor(tensor,
                               flat_product(tensor.layout(), tiler));
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
constexpr auto tile_to_shape(Layout<Shape, Stride> const& block,
                             TargetShape const& target_shape) {
  // TODO(stage5): This is a simplified logical_divide-based implementation.
  // It does not yet preserve the full block layout semantics of CuTe's
  // tile_to_shape (e.g. arbitrary block strides and mode orders).
  auto target_layout = make_layout(target_shape);
  return flat_divide(target_layout, shape(block));
}

template <class Engine, class Layout, class TileShape, class TileCoord>
constexpr auto local_tile(Tensor<Engine, Layout>& tensor,
                          TileShape const& tiler,
                          TileCoord const& coord) {
  // Basic inner partitioning:
  //   zipped_divide(tensor, tiler) -> (Tile, Rest)
  //   keep the complete Tile and select one coordinate from Rest.
  auto tiled = zipped_divide(tensor, tiler);
  auto tile_coord = repeat_like(get<0>(shape(tiled)), _);
  return tiled(tile_coord, coord);
}

template <class Engine, class Layout, class TileShape, class TileCoord>
constexpr auto local_tile(Tensor<Engine, Layout> const& tensor,
                          TileShape const& tiler,
                          TileCoord const& coord) {
  auto tiled = zipped_divide(tensor, tiler);
  auto tile_coord = repeat_like(get<0>(shape(tiled)), _);
  return tiled(tile_coord, coord);
}

template <class Engine, class Layout, class TileShape, class TileCoord>
constexpr auto local_tile_clamped(Tensor<Engine, Layout>&, TileShape const&,
                                  TileCoord const&) {
  static_assert(detail::always_false_v<Engine, Layout, TileShape, TileCoord>,
                "TODO(stage5): implement local_tile_clamped");
}

}  // namespace minicute
