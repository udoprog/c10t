#include "engine/block_rotation.hpp"

block_rotation::block_rotation(int rotation)
    : rotation(rotation)
{
}

void block_rotation::transform_xz(int& x, int& z) {
  int t = x;

  switch (rotation) {
    case 270:
      x = 15 - z;
      z = t;
      break;
    case 180:
      z = 15 - z;
      x = 15 - x;
      break;
    case 90:
      x = z;
      z = 15 - t;
      break;
  };
}
