// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/image_operations.hpp"
#include <string.h>

#include <iostream>
  
image_operations::image_operations() : minx(0), miny(0), maxx(0), maxy(0) { }
image_operations::~image_operations() { }

void image_operations::add_pixel(int x, int y, color &c)
{
  if (c.is_invisible()) {
    return;
  }
  
  if (!(x >= minx)) { return; }
  if (!(y >= miny)) { return; }
  if (!(x < maxx)) { return; }
  if (!(y < maxy)) { return; }
  
  image_operation oper;
  
  oper.x = (uint16_t)x;
  oper.y = (uint16_t)y;
  //oper.order = ++order;
  oper.c = c;
  
  if (!oper.c.is_transparent()) {
    uint64_t p = oper.x + oper.y * maxx;
    
    uint8_t pb = lookup[p / 8];
    
    if (((pb >> (p % 8)) & 0x01) > 0) {
      return;
    }
    
    pb |= (0x1 << (p % 8));
    lookup[p / 8] = pb;
  }
  
  operations.push_back(oper);
}

void image_operations::set_limits(int x, int y) 
{
  minx = 0;
  miny = 0;
  maxx = x;
  maxy = y;
  
  size_t lookup_size = (maxx * maxy) / 8 + 1;
  lookup.reset(new uint8_t[lookup_size]);
  memset(lookup.get(), 0x0, lookup_size);
  operations.reserve(maxx * maxy * 2);
}

