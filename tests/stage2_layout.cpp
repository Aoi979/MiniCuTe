#include "../include/minicute.hpp"

#include <cassert>

using namespace minicute;

int main() {
  constexpr auto col = make_layout(make_shape(_4{}, _5{}));
  static_assert(stride(col) == make_stride(_1{}, _4{}));
  static_assert(col(_2{}, _3{}) == _14{});
  static_assert(cosize(col) == Int<20>{});

  constexpr auto row = make_layout(make_shape(_4{}, _5{}), LayoutRight{});
  static_assert(stride(row) == make_stride(_5{}, _1{}));
  static_assert(row(_2{}, _3{}) == _13{});
  static_assert(idx2crd(_13{}, row.shape(), row.stride()) ==
                make_coord(_2{}, _3{}));

  constexpr auto nested = make_layout(make_shape(make_shape(_2{}, _3{}), _4{}));
  static_assert(nested(make_coord(make_coord(_1{}, _2{}), _3{})) == _23{});

  auto dyn = make_layout(make_shape(4, 5));
  assert(dyn(2, 3) == 14);

  auto [sub_layout, offset] = slice_and_offset(make_coord(_, 3), dyn);
  assert(offset == 12);
  assert(sub_layout(2) + offset == dyn(2, 3));

  constexpr auto coal = coalesce(col);
  static_assert(shape(coal) == Int<20>{});
  static_assert(coal(_14{}) == _14{});
}
