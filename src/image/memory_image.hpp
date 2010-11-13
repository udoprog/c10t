// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef MEMORY_IMAGE
#define MEMORY_IMAGE

#include "image/image_base.hpp"
#include "image/color.hpp"

class memory_image : public image_base {
private:
  uint8_t *colors;
public:
  memory_image(int w, int h);
  
  ~memory_image();
  
  void blend_pixel(pos_t x, pos_t y, color &c);
  void set_pixel(pos_t x, pos_t y, color&);
  void get_pixel(pos_t x, pos_t y, color&);
  void get_line(pos_t y, pos_t offset, pos_t width, color*);
};

#endif /* MEMORY_IMAGE */
