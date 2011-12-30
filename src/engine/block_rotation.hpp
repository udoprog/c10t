#ifndef _ENGINE_BLOCK_ROTATION_HPP
#define _ENGINE_BLOCK_ROTATION_HPP

#include <stdint.h>

#include <boost/shared_ptr.hpp>

#include "nbt/types.hpp"

class block_rotation {
public:
  block_rotation(int rotation, boost::shared_ptr<nbt::ByteArray> byte_array);

  void set_xz(int x, int z);
  
  /**
   * Blocks[ z + ( y * ChunkSizeY(=128) + ( x * ChunkSizeY(=128) * ChunkSizeZ(=16) ) ) ]; 
   */
  uint8_t get8(int y);
  
  uint8_t get8();
  
  int get4(int y);
private:
  int rotation;
  boost::shared_ptr<nbt::ByteArray> byte_array;
  
  int x, z;

  void transform_xz(int& x, int& z);
};

/* For backwards compatibility */
typedef block_rotation BlockRotation;

#endif /* _ENGINE_BLOCK_ROTATION_HPP */
