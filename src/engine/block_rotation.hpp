#ifndef _ENGINE_BLOCK_ROTATION_HPP
#define _ENGINE_BLOCK_ROTATION_HPP

#include <stdint.h>

#include <boost/shared_ptr.hpp>

#include "nbt/types.hpp"
#include "mc/blocks.hpp"

class block_rotation {
public:
  block_rotation(int rotation, boost::shared_ptr<nbt::ByteArray> array);

  void set_xz(int x, int z);
  int get8(int y);
  int get4(int y);
  void transform_xz(int& x, int& z);
private:
  int x, z;
  int rotation;
  boost::shared_ptr<nbt::ByteArray> array;
};

#endif /* _ENGINE_BLOCK_ROTATION_HPP */
