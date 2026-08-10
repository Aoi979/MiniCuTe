#include "../include/minicute.hpp"

#include <array>
#include <cassert>

using namespace minicute;

int main() {
  std::array<int, 120> storage{};
  for (int i = 0; i < static_cast<int>(storage.size()); ++i) {
    storage[static_cast<std::size_t>(i)] = i;
  }

  auto tensor = make_tensor(storage, make_layout(make_shape(_8{}, _9{})));
  auto tiler = make_shape(_2{}, _3{});

  auto composed = composition(tensor, make_layout(make_shape(_8{}, _9{})));
  assert(shape(composed) == make_shape(_8{}, _9{}));

  auto logical = logical_divide(tensor, tiler);
  assert(shape(logical) ==
         make_shape(make_shape(_2{}, _4{}), make_shape(_3{}, _3{})));

  auto zipped = zipped_divide(tensor, tiler);
  assert(shape(zipped) ==
         make_shape(make_shape(_2{}, _3{}), make_shape(_4{}, _3{})));
  auto zipped_coord = make_coord(make_coord(_1{}, _2{}),
                                 make_coord(_2{}, _1{}));
  assert(zipped(zipped_coord) == 45);
  zipped(zipped_coord) = 123;
  assert(storage[45] == 123);

  auto tiled = tiled_divide(tensor, tiler);
  assert(shape(tiled) == make_shape(make_shape(_2{}, _3{}), _4{}, _3{}));

  auto flat = flat_divide(tensor, tiler);
  assert(shape(flat) == make_shape(_2{}, _3{}, _4{}, _3{}));

  auto local = local_tile(tensor, tiler, make_coord(_2{}, _1{}));
  assert(shape(local) == make_shape(_2{}, _3{}));
  assert(local(_0{}, _0{}) == tensor(_4{}, _3{}));

  std::array<int, 25> edge_storage{};
  for (int i = 0; i < static_cast<int>(edge_storage.size()); ++i) {
    edge_storage[static_cast<std::size_t>(i)] = i;
  }
  auto edge_tensor = make_tensor(edge_storage, make_layout(make_shape(5, 5)));
  auto edge = local_tile(edge_tensor, make_shape(2, 2), make_coord(2, 2));
  assert(shape(edge) == make_shape(2, 2));
  assert(edge(0, 0) == edge_tensor(4, 4));

  auto block = make_tensor(storage, make_layout(make_shape(_2{}, _3{})));
  auto product_tiler = make_layout(make_shape(_4{}, _5{}));

  auto logical_product_tensor = logical_product(block, product_tiler);
  assert(shape(logical_product_tensor) ==
         make_shape(make_shape(_2{}, _3{}), make_shape(_4{}, _5{})));

  auto zipped_product_tensor = zipped_product(block, product_tiler);
  assert(shape(zipped_product_tensor) ==
         make_shape(make_shape(_2{}, _3{}), make_shape(_4{}, _5{})));

  auto tiled_product_tensor = tiled_product(block, product_tiler);
  assert(shape(tiled_product_tensor) ==
         make_shape(make_shape(_2{}, _3{}), _4{}, _5{}));

  auto flat_product_tensor = flat_product(block, product_tiler);
  assert(shape(flat_product_tensor) ==
         make_shape(_2{}, _3{}, _4{}, _5{}));
}
