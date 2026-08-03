#include "../include/minicute.hpp"

#include <array>
#include <cassert>
#include <iostream>
#include <sstream>

using namespace minicute;

int main() {
  std::array<int, 20> storage{};
  for (int i = 0; i < static_cast<int>(storage.size()); ++i) {
    storage[static_cast<std::size_t>(i)] = i;
  }

  auto tensor = make_tensor(storage, make_layout(make_shape(4, 5)));
  assert(tensor(2, 3) == 14);
  tensor(1, 4) = 99;
  assert(storage[17] == 99);

  auto col3 = tensor(_, 3);
  assert(shape(col3) == 4);
  assert(col3(2) == 14);
  col3(0) = -7;
  assert(tensor(0, 3) == -7);

  auto dst = make_array_tensor<int>(make_layout(make_shape(_4{}, _5{})));
  clear(dst);
  assert(dst(_2{}, _3{}) == 0);
  fill(dst, 5);
  assert(dst(_2{}, _3{}) == 5);
  copy(tensor, dst);
  assert(dst(1, 4) == 99);

  int visited = 0;
  for_each(dst, [&](auto const&, int&) { ++visited; });
  assert(visited == 20);

  std::ostringstream os;
  print_tensor(os, dst);
  assert(os.str().find("Tensor") != std::string::npos);
  print_tensor(std::cout, dst) << "\n";
}
