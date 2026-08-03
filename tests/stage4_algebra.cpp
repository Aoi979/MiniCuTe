#include "../include/minicute.hpp"

#include <array>
#include <cassert>
#include <sstream>

using namespace minicute;

int main() {
  static_assert(crd2idx(make_coord(_1{}, _2{}), make_shape(_2{}, _3{})) ==
                _5{});
  static_assert(crd2crd(_5{}, _6{}, make_shape(_2{}, _3{})) ==
                make_coord(_1{}, _2{}));

  constexpr auto permuted = make_layout(make_shape(_2{}, _3{}),
                                        make_stride(_3{}, _1{}));
  constexpr auto rinv = right_inverse(permuted);
  static_assert(permuted(rinv(_4{})) == _4{});

  constexpr auto linv = left_inverse(permuted);
  static_assert(linv(permuted(_1{}, _2{})) == _5{});

  constexpr auto sparse = make_layout(make_shape(_2{}), make_stride(_2{}));
  constexpr auto comp = complement(sparse, _6{});
  static_assert(shape(comp) == _4{});
  static_assert(comp(_0{}) == _1{});
  static_assert(comp(_1{}) == _3{});

  auto layout = make_layout(make_shape(4, 5));
  auto [sliced, offset] = domain_offset(make_coord(_, 3), layout);
  assert(offset == 12);
  assert(sliced(2) + offset == layout(2, 3));

  auto fixed = dice(make_coord(_, 3), layout);
  assert(shape(fixed) == 5);

  std::array<int, 20> storage{};
  auto tensor = make_tensor(storage, layout);
  std::ostringstream os;
  print_tensor(os, tensor);
  assert(os.str().find("Tensor") != std::string::npos);
}
