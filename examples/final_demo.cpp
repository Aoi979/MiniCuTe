#include "../include/minicute.hpp"

#include <array>
#include <iostream>

using namespace minicute;

int main() {
  auto layout = make_layout(make_shape(4, 5));
  std::array<int, 20> storage{};
  for (int i = 0; i < static_cast<int>(storage.size()); ++i) {
    storage[static_cast<std::size_t>(i)] = i;
  }

  auto tensor = make_tensor(storage, layout);
  auto col3 = tensor(_, 3);
  auto tiles = zipped_divide(tensor, make_shape(2, 2));

  std::cout << "tensor(2,3): " << tensor(2, 3) << "\n";
  std::cout << "tensor(_,3)(2): " << col3(2) << "\n";
  std::cout << "zipped tile ((1,1),(1,1)): "
            << tiles(make_coord(make_coord(1, 1), make_coord(1, 1))) << "\n";
}
