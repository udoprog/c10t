// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/image_operations.hpp"
#include <string.h>

#include <iostream>
  
image_operations::image_operations() : min_x(0), min_y(0), max_x(0), max_y(0) { }
image_operations::~image_operations() { }

void image_operations::add_pixel(pos_t x, pos_t y, color &c)
{
  if (c.is_invisible()) {
    return;
  }
  
  if (!(x >= min_x)) { return; }
  if (!(y >= min_y)) { return; }
  if (!(x < max_x)) { return; }
  if (!(y < max_y)) { return; }
  
  image_operation oper;
  
  oper.x = (uint16_t)x;
  oper.y = (uint16_t)y;
  //oper.order = ++order;
  oper.c = c;
  
  if (!oper.c.is_transparent()) {
    uint64_t p = oper.x + oper.y * max_x;
    
    uint8_t pb = lookup[p / 8];
    
    if (((pb >> (p % 8)) & 0x01) > 0) {
      return;
    }
    
    pb |= (0x1 << (p % 8));
    lookup[p / 8] = pb;
  }
  
  operations.push_back(oper);
}

void image_operations::set_limits(pos_t x, pos_t y) 
{
  min_x = 0;
  min_y = 0;
  max_x = x;
  max_y = y;
  
  size_t lookup_size = (max_x * max_y) / 8 + 1;
  lookup.reset(new uint8_t[lookup_size]);
  memset(lookup.get(), 0x0, lookup_size);
  operations.reserve(max_x * max_y * 2);
}

