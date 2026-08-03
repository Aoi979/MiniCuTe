#include "../include/minicute.hpp"

using namespace minicute;

int main() {
  static_assert(C<7>::value == 7);
  auto shape = make_shape(2, 3);
  auto stride = make_stride(1, 2);
  auto layout = make_layout(shape, stride);
  (void)layout;
}
