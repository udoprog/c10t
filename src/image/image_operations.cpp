// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/image_operations.hpp"
#include <string.h>

#include <boost/foreach.hpp>

#include <algorithm>
  
image_operations::image_operations(uint32_t z)
    : z(z), min_x(0), min_y(0), max_x(0), max_y(0)
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

  color n = color(c);

  // tag with depth.
  n.z = z;
  
  image_operation oper;
  
  oper.x = (uint16_t)x;
  oper.y = (uint16_t)y;
  oper.c = n;
  
  operations.push_back(oper);
}

void image_operations::optimize()
{
  bool* blocked = new bool[max_x*max_y];

  for (int i = 0; i < max_x*max_y; i++)
  {
    blocked[i] = false;
  }

  operations_vector::reverse_iterator iter = operations.rbegin();
  operations_vector new_operations;

  while (iter != operations.rend()) {
    image_operation operation = *iter;
    iter++;

    int offset = operation.y * max_x + operation.x;
    
    if (blocked[offset]) {
      continue;
    }

    blocked[offset] = operation.c.is_opaque();
    new_operations.push_back(operation);
  }

  std::reverse(new_operations.begin(), new_operations.end());

  operations = new_operations;

  delete [] blocked;
}

void image_operations::set_limits(pos_t x, pos_t y) 
{
  min_x = 0;
  min_y = 0;
  max_x = x;
  max_y = y;
  
  operations.reserve(max_x * max_y * 2);
}

