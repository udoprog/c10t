#ifndef _ENGINE_BLOCK_ROTATION_HPP
#define _ENGINE_BLOCK_ROTATION_HPP

#include <stdint.h>

#include <boost/shared_ptr.hpp>

#include "nbt/types.hpp"
#include "mc/blocks.hpp"

template<typename T, typename V>
class block_rotation {
public:
  block_rotation(
      int rotation,
      boost::shared_ptr<T> array)
      : x(0), z(0), rotation(rotation), array(array)
  {
  }

  void set_xz(int x, int z) {
    transform_xz(x, z);
    this->x = x;
    this->z = z;
  }

  /**
   * Blocks[ z + ( y * ChunkSizeY(=128) + ( x * ChunkSizeY(=128) * ChunkSizeZ(=16) ) ) ]; 
   */
  V get8(int y) {
    int p = y + (z * mc::MapY) + (x * mc::MapY * mc::MapZ);
    if (!(p >= 0 && p < array->length)) return -1;
    return array->values[p];
  }

  V get8() {
    int p = x + (z * mc::MapX);
    assert (p >= 0 && p < array->length);
    return array->values[p];
  }

  int get4(int y) {
    int p = (y + (z * mc::MapY) + (x * mc::MapY * mc::MapZ)) >> 1;
    if (!(p >= 0 && p < array->length)) return -1;
    return ((array->values[p]) >> ((y % 2) * 4)) & 0xf;
  }

  void transform_xz(int& x, int& z) {
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
private:
  int x, z;
  int rotation;
  boost::shared_ptr<T> array;
};

#endif /* _ENGINE_BLOCK_ROTATION_HPP */
