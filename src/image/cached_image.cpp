// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/cached_image.hpp"

// cached_image
void cached_image::set_pixel(size_t x, size_t y, color& c)
{
  if (!(x < get_width())) { return; }
  if (!(y < get_height())) { return; }
  fs.seekp(get_offset(x, y), std::ios::beg);
  fs.write(reinterpret_cast<char*>(&c), sizeof(color));
}

void cached_image::get_pixel(size_t x, size_t y, color& c)
{
  if (!(x < get_width())) { return; }
  if (!(y < get_height())) { return; }
  fs.seekg(get_offset(x, y), std::ios::beg);
  fs.read(reinterpret_cast<char*>(&c), sizeof(color));
}

void cached_image::get_line(size_t y, size_t offset, size_t width, color* c)
{
  if (!(y < get_height())) { return; }
  if (!(offset < get_width())) { return; }
  if (!(width + offset < get_width())) { width = get_width() - offset; }
  
  fs.seekg(get_offset(offset, y), std::ios::beg);
  fs.read(reinterpret_cast<char*>(c), sizeof(color) * width);
}

void cached_image::set_line(size_t y, size_t offset, size_t width, color* c)
{
  if (!(y < get_height())) { return; }
  if (!(offset < get_width())) { return; }
  if (!(width + offset < get_width())) { width = get_width() - offset; }
  
  fs.seekp(get_offset(offset, y), std::ios::beg);
  fs.write(reinterpret_cast<char*>(c), sizeof(color) * width);
}

void cached_image::flush_buffer()
{
  if (buffer_set)
  {
    for (size_t y = 0; y < buffer_h; y++) {
      set_line(buffer_y + y, buffer_x, buffer_w, &buffer[y * buffer_w]);
    }
  }
}

void cached_image::read_buffer()
{
  for (size_t y = 0; y < buffer_h; y++) {
    get_line(buffer_y + y, buffer_x, buffer_w, &buffer[y * buffer_w]);
  }
}

void cached_image::blend_pixel(size_t x, size_t y, color &c)
{
  // do nothing if color is invisible
  if (c.is_invisible()) {
    return;
  }
  
  size_t bx = x - buffer_x;
  size_t by = y - buffer_y;
  size_t bp = bx + by * buffer_w;

  assert(bp < buffer_s);
  
  buffer[bp].blend(c);
}


void cached_image::align(size_t x, size_t y, size_t w, size_t h)
{
  flush_buffer();
  
  if (!(buffer_s <= w * h)) {
    buffer.reset(new color[w * h]);
    buffer_s = w * h;
  }
  
  buffer_x = x, buffer_y = y;
  buffer_w = w, buffer_h = h;
  
  read_buffer();
  
  buffer_set = true;
}
