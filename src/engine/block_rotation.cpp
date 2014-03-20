#include "engine/block_rotation.hpp"

block_rotation::block_rotation(
    int rotation,
    boost::shared_ptr<nbt::ByteArray> array)
    : x(0), z(0), rotation(rotation), array(array)
{
}

void block_rotation::set_xz(int x, int z) {
  transform_xz(x, z);
  this->x = x;
  this->z = z;
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

/**
 */
int block_rotation::get8(int y, int d) {
  int p = ((y * 16 + z) * 16 + x);
  if (!(p >= 0 && p < array->length)) return d;
  return array->values[p] & 0xff;
}

int block_rotation::get4(int y, int d) {
  int p = ((y * 16 + z) * 16 + x) >> 1;
  if (!(p >= 0 && p < array->length)) return d;
  return ((array->values[p]) >> ((y % 2) * 4)) & 0xf;
}
