// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/image_operations.hpp"
#include <string.h>
  
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
    size_t p = oper.x + oper.y * maxx;
    
    if (lookup[p]) {
      return;
    }
    
    lookup[p] = true;
  }
  
  operations.push_back(oper);
}

void image_operations::set_limits(int x, int y) 
{
  minx = 0;
  miny = 0;
  maxx = x;
  maxy = y;
  
  lookup.reset(new bool[maxx * maxy]);
  memset(lookup.get(), 0x0, sizeof(bool) * maxx * maxy);
  operations.reserve(maxx * maxy * 2);
}

