// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/memory_image.hpp"

#include <string.h>

// memory_image
memory_image::memory_image(int w, int h) : image_base(w, h)
{
  colors = new uint8_t[sizeof(color) * w * h];
  memset(colors, 0x0, sizeof(color) * w * h);
}

memory_image::~memory_image()
{
  delete [] colors;
}

void memory_image::set_pixel(size_t x, size_t y, color &c)
{
  if (!(x < get_width())) { return; }
  if (!(y < get_height())) { return; }
  c.write(this->colors + get_offset(x, y));
}

void memory_image::get_pixel(size_t x, size_t y, color &c)
{
  if (!(x < get_width())) { return; }
  if (!(y < get_height())) { return; }
  c.read(this->colors + get_offset(x, y));
}

void memory_image::get_line(size_t y, size_t offset, size_t width, color* c)
{
  if (!(y < get_height())) { return; }
  if (!(offset < get_width())) { return; }
  if (!(width + offset < get_width())) { width = get_width() - offset; }
  
  memcpy(c, this->colors + get_offset(offset, y), width * sizeof(color));
}

void memory_image::blend_pixel(size_t x, size_t y, color &c)
{
  color o;
  get_pixel(x, y, o);
  
  if (o.is_invisible()) {
    set_pixel(x, y, c);
    return;
  }
  
  o.blend(c);
  set_pixel(x, y, o);
}
