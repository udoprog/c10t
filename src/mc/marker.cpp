#include "mc/marker.hpp"

namespace mc {
  marker::marker(std::string text, int x, int y, int z) :
      text(text), x(x), y(y), z(z)
  {
  }

  std::string marker::get_text() {
    return text;
  }

  int marker::get_x() {
    return x;
  }

  int marker::get_y() {
    return y;
  }

  int marker::get_z() {
    return z;
  }
}
