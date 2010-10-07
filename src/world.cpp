// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "world.h"

void transform_world_xz(int& x, int& z, int rotation)
{
  int t = x;
  switch (rotation) {
    case 270:
      x = z;
      z = -t;
      break;
    case 180:
      x = -x;
      z = -z;
      break;
    case 90:
      x = -z;
      z = t;
      break;
  }
}
