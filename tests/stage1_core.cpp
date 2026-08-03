#include "../include/minicute.hpp"

using namespace minicute;

int main() {
  static_assert((_2{} + _3{}) == _5{});
  static_assert((_4{} * _3{}) == _12{});
  static_assert((_8{} / _2{}) == _4{});
  static_assert(product(make_shape(_2{}, _3{}, _4{})) == _24{});
  static_assert(sum(make_shape(_2{}, _3{}, _4{})) == _9{});
  static_assert(inner_product(make_shape(_2{}, _3{}),
                              make_stride(_1{}, _4{})) == _14{});

  constexpr auto nested = make_shape(make_shape(_2{}, _3{}), _4{});
  static_assert(rank(nested) == _2{});
  static_assert(depth(nested) == _2{});
  static_assert(size(nested) == _24{});
  static_assert(flatten(nested) == make_shape(_2{}, _3{}, _4{}));
  static_assert(unflatten(flatten(nested), nested) == nested);

  static_assert(append(make_shape(_2{}, _3{}), _4{}) ==
                make_shape(_2{}, _3{}, _4{}));
  static_assert(prepend(make_shape(_2{}, _3{}), _1{}) ==
                make_shape(_1{}, _2{}, _3{}));
  static_assert(replace<1>(make_shape(_2{}, _3{}, _4{}), _9{}) ==
                make_shape(_2{}, _9{}, _4{}));
  static_assert(group<1, 3>(make_shape(_2{}, _3{}, _4{}, _5{})) ==
                make_shape(_2{}, make_shape(_3{}, _4{}), _5{}));
  static_assert(repeat<3>(_1{}) == make_shape(_1{}, _1{}, _1{}));
  static_assert(product_each(nested) == make_shape(_6{}, _4{}));
  static_assert(shape_div(make_shape(_8{}, _9{}),
                          make_shape(_4{}, _4{})) == make_shape(_2{}, _3{}));
}
