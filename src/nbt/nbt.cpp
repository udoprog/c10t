// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "nbt.h"

bool nbt::is_big_endian() {
  int32_t i = 1;
  return ((int8_t*)(&i))[0] == 0;
}
