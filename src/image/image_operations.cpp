// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/image_operations.hpp"
#include <string.h>

#include <iostream>
  
image_operations::image_operations()
    : min_x(0), min_y(0), max_x(0), max_y(0)
{
}

image_operations::~image_operations()
{
}

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
  
  operations.push_back(oper);
}

void image_operations::set_limits(pos_t x, pos_t y) 
{
  min_x = 0;
  min_y = 0;
  max_x = x;
  max_y = y;
  
  operations.reserve(max_x * max_y * 2);
}

