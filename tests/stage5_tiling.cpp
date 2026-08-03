#include "../include/minicute.hpp"

#include <array>
#include <cassert>

using namespace minicute;

int main() {
  auto ordered = make_ordered_layout(make_shape(_2{}, _3{}, _4{}),
                                     make_shape(_2{}, _0{}, _1{}));
  static_assert(stride(ordered) == make_stride(_12{}, _1{}, _3{}));
  static_assert(ordered(_1{}, _2{}, _3{}) == Int<23>{});

  auto big = make_layout(make_shape(_8{}, _9{}));
  auto logical = logical_divide(big, make_shape(_2{}, _3{}));
  static_assert(shape(logical) == make_shape(make_shape(_2{}, _4{}),
                                             make_shape(_3{}, _3{})));
  static_assert(logical(make_coord(make_coord(_1{}, _2{}),
                                   make_coord(_2{}, _1{}))) == Int<45>{});

  auto zipped = zipped_divide(big, make_shape(_2{}, _3{}));
  static_assert(shape(zipped) == make_shape(make_shape(_2{}, _3{}),
                                            make_shape(_4{}, _3{})));
  static_assert(zipped(make_coord(make_coord(_1{}, _2{}),
                                  make_coord(_2{}, _1{}))) == Int<45>{});

  auto blocked = blocked_product(make_layout(make_shape(_2{}, _3{})),
                                 make_layout(make_shape(_4{}, _5{})));
  static_assert(shape(blocked) == make_shape(make_shape(_2{}, _4{}),
                                             make_shape(_3{}, _5{})));
  static_assert(blocked(make_coord(make_coord(_1{}, _3{}),
                                   make_coord(_2{}, _4{}))) == Int<119>{});

  auto raked = raked_product(make_layout(make_shape(_2{}, _3{})),
                             make_layout(make_shape(_4{}, _5{})));
  static_assert(shape(raked) == make_shape(make_shape(_4{}, _2{}),
                                           make_shape(_5{}, _3{})));
  static_assert(raked(make_coord(make_coord(_3{}, _1{}),
                                 make_coord(_4{}, _2{}))) == Int<119>{});

  auto tiled_to_shape = tile_to_shape(make_layout(make_shape(_2{}, _3{})),
                                      make_shape(_8{}, _9{}));
  static_assert(shape(tiled_to_shape) == make_shape(_2{}, _3{}, _4{}, _3{}));

  std::array<int, 25> storage{};
  for (int i = 0; i < static_cast<int>(storage.size()); ++i) {
    storage[static_cast<std::size_t>(i)] = i;
  }
  auto tensor = make_tensor(storage, make_layout(make_shape(5, 5)));
  auto edge = local_tile_clamped(tensor, make_shape(2, 2), make_coord(2, 2));
  assert(shape(edge) == make_shape(1, 1));
  assert(edge(0, 0) == tensor(4, 4));
}
