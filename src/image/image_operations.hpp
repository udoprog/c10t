// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef IMAGE_OPERATIONS
#define IMAGE_OPERATIONS

#include <cstdlib>
#include <vector>

#include <boost/shared_array.hpp>

#include "image/color.hpp"

struct image_operation {
  color c;
  int depth;
  uint16_t x, y;
};

class image_operations {
private:
  int maxx, maxy;
  boost::shared_array<bool> lookup;
public:
  std::vector<image_operation> operations;
  
  void add_pixel(int x, int y, color &c);
  void set_limits(int x, int y);
  
  image_operations();
  ~image_operations();
};


#endif /* IMAGE_OPERATIONS */
