#include "engine/block_rotation.hpp"

#include "mc/blocks.hpp"

block_rotation::block_rotation(
    int rotation,
    boost::shared_ptr<nbt::ByteArray> byte_array)
    : rotation(rotation), byte_array(byte_array)
{
}

void block_rotation::set_xz(int x, int z) {
  transform_xz(x, z);
  this->x = x;
  this->z = z;
}

/**
 * Blocks[ z + ( y * ChunkSizeY(=128) + ( x * ChunkSizeY(=128) * ChunkSizeZ(=16) ) ) ]; 
 */
uint8_t block_rotation::get8(int y) {
  int p = y + (z * mc::MapY) + (x * mc::MapY * mc::MapZ);
  if (!(p >= 0 && p < byte_array->length)) return -1;
  return byte_array->values[p];
}

uint8_t block_rotation::get8() {
  int p = x + (z * mc::MapX);
  assert (p >= 0 && p < byte_array->length);
  return byte_array->values[p];
}

int block_rotation::get4(int y) {
  int p = (y + (z * mc::MapY) + (x * mc::MapY * mc::MapZ)) >> 1;
  if (!(p >= 0 && p < byte_array->length)) return -1;
  return ((byte_array->values[p]) >> ((y % 2) * 4)) & 0xf;
}

void block_rotation::transform_xz(int& x, int& z) {
  int t = x;
  switch (rotation) {
    case 270:
      x = mc::MapX - z - 1;
      z = t;
      break;
    case 180:
      z = mc::MapZ - z - 1;
      x = mc::MapX - x - 1;
      break;
    case 90:
      x = z;
      z = mc::MapZ - t - 1;
      break;
  };
}
