// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef IMAGE_OPERATIONS
#define IMAGE_OPERATIONS

#include <cstdlib>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include "image/color.hpp"

typedef uint64_t pos_t;

class image_operations;

typedef boost::shared_ptr<image_operations> image_operations_ptr;

struct image_operation {
  color c;
  uint16_t x, y;
};

class image_operations
{
public:
  pos_t min_x, min_y;
  pos_t max_x, max_y;
  
  typedef std::vector<image_operation> operations_vector;

  operations_vector operations;
  
  void add_pixel(pos_t x, pos_t y, color &c);
  void set_limits(pos_t x, pos_t y);
  void optimize();
  
  image_operations();
  ~image_operations();
};


#endif /* IMAGE_OPERATIONS */
