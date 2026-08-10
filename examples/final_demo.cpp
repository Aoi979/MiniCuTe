#include "../include/minicute.hpp"

#include <array>
#include <iostream>

using namespace minicute;

template <class LayoutOrTensor>
void print_shape_line(char const* name, LayoutOrTensor const& value) {
  std::cout << name << " shape=";
  detail::print_value(std::cout, shape(value)) << "\n";
}

int main() {
  std::cout << "== MiniCuTe demo ==\n\n";

  // Static integer tuples and compile-time layout mapping.
  constexpr auto ordered = make_ordered_layout(
      make_shape(_2{}, _3{}, _4{}), make_shape(_2{}, _0{}, _1{}));
  static_assert(ordered(_1{}, _2{}, _3{}) == Int<23>{});
  std::cout << "ordered layout (1,2,3) -> "
            << ordered(_1{}, _2{}, _3{}) << "\n";

  constexpr auto matrix_layout = make_layout(make_shape(_4{}, _5{}));
  static_assert(matrix_layout(_2{}, _3{}) == Int<14>{});
  std::cout << "column-major layout (2,3) -> "
            << matrix_layout(_2{}, _3{}) << "\n\n";

  // Tensor storage, indexing, slicing, and mutation.
  std::array<int, 20> matrix_storage{};
  for (int i = 0; i < static_cast<int>(matrix_storage.size()); ++i) {
    matrix_storage[static_cast<std::size_t>(i)] = i;
  }

  auto matrix = make_tensor(matrix_storage, matrix_layout);
  auto column = matrix(_, 3);
  column(_2{}) = 99;

  std::cout << "matrix(2,3): " << matrix(_2{}, _3{}) << "\n";
  std::cout << "column 3 after column(2)=99: " << column(_2{}) << "\n";

  auto [column_layout, column_offset] =
      domain_offset(make_coord(_, 3), matrix.layout());
  std::cout << "column view offset: " << column_offset << "\n";
  std::cout << "column view shape: ";
  detail::print_value(std::cout, shape(column_layout)) << "\n\n";

  // Layout composition changes the logical view while reusing the same data.
  auto composed = composition(matrix, make_layout(make_shape(4, 5)));
  print_shape_line("composed tensor", composed);
  std::cout << "composed(2,3): " << composed(2, 3) << "\n\n";

  // Divide a tensor into logical tiles. All of these are views over the same
  // storage; only their Layout differs.
  std::array<int, 120> tiled_storage{};
  for (int i = 0; i < static_cast<int>(tiled_storage.size()); ++i) {
    tiled_storage[static_cast<std::size_t>(i)] = i;
  }

  auto global = make_tensor(tiled_storage,
                            make_layout(make_shape(_8{}, _9{})));
  auto tiler = make_shape(_2{}, _3{});

  auto logical = logical_divide(global, tiler);
  auto zipped = zipped_divide(global, tiler);
  auto tiled = tiled_divide(global, tiler);
  auto flat = flat_divide(global, tiler);

  print_shape_line("logical_divide", logical);
  print_shape_line("zipped_divide", zipped);
  print_shape_line("tiled_divide", tiled);
  print_shape_line("flat_divide", flat);

  auto tile = local_tile(global, tiler, make_coord(_2{}, _1{}));
  print_shape_line("local_tile(2,1)", tile);
  std::cout << "local tile values: ";
  print_tensor(std::cout, tile) << "\n";
  tile(_0{}, _0{}) = 777;
  std::cout << "global(4,3) after tile write: " << global(_4{}, _3{})
            << "\n\n";

  // Product operations build repeated block layouts. A Layout tiler is used
  // here because it makes the tile and rest modes explicit.
  auto block = make_tensor(tiled_storage,
                           make_layout(make_shape(_2{}, _3{})));
  auto product_tiler = make_layout(make_shape(_4{}, _5{}));

  auto logical_product_tensor = logical_product(block, product_tiler);
  auto zipped_product_tensor = zipped_product(block, product_tiler);
  auto tiled_product_tensor = tiled_product(block, product_tiler);
  auto flat_product_tensor = flat_product(block, product_tiler);

  print_shape_line("logical_product", logical_product_tensor);
  print_shape_line("zipped_product", zipped_product_tensor);
  print_shape_line("tiled_product", tiled_product_tensor);
  print_shape_line("flat_product", flat_product_tensor);

  // Basic algorithms operate on the same Tensor abstraction.
  auto result = make_array_tensor<int>(make_layout(make_shape(_4{}, _5{})));
  clear(result);
  fill(result, 7);
  copy(matrix, result);
  std::cout << "copied result: ";
  print_tensor(std::cout, result) << "\n";
}
