// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/memory_image.hpp"

#include <string.h>

// memory_image
memory_image::memory_image(int w, int h) : image_base(w, h)
{
  colors = new color[w * h];
  memset(colors, 0x255, w * h);
}

memory_image::~memory_image()
{
  delete [] colors;
}

void memory_image::set_pixel(pos_t x, pos_t y, color &c)
{
  if (!(x < get_width())) { return; }
  if (!(y < get_height())) { return; }
  c.write(this->colors + get_offset(x, y));
}

void memory_image::get_pixel(pos_t x, pos_t y, color &c)
{
  if (!(x < get_width())) { return; }
  if (!(y < get_height())) { return; }
  c.read(this->colors + get_offset(x, y));
}

void memory_image::get_line(pos_t y, pos_t offset, pos_t width, color* c)
{
  if (!(y < get_height())) { return; }
  if (!(offset < get_width())) { return; }
  if (!(width + offset < get_width())) { width = get_width() - offset; }
  
  memcpy(c, this->colors + get_offset(offset, y), width * sizeof(color));
}

void memory_image::blend_pixel(pos_t x, pos_t y, color &c)
{
  color o;
  get_pixel(x, y, o);

  if (c.z < o.z) {
    c.blend(o);
    set_pixel(x, y, c);
  }
  else {
    o.blend(c);
    set_pixel(x, y, o);
  }
}
