#ifndef _ENGINE_BLOCK_ROTATION_HPP
#define _ENGINE_BLOCK_ROTATION_HPP

#include <stdint.h>

#include <boost/shared_ptr.hpp>

#include "nbt/types.hpp"
#include "mc/blocks.hpp"

class block_rotation {
public:
  block_rotation(int rotation);

  void transform_xz(int& x, int& z);

private:
  int rotation;

};

#endif /* _ENGINE_BLOCK_ROTATION_HPP */
