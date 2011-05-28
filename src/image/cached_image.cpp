// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.

#include "image/cached_image.hpp"

#include "algorithm.hpp"

cached_image::cached_image(const fs::path path, pos_t w, pos_t h, pos_t l_w, pos_t l_h) :
  image_base(w, h),
  path(path),
  buffer_s((l_w + 1) * l_h),
  buffer_set(false),
  buffer(new color[buffer_s])
{
  using namespace ::std;
  fs.exceptions(ios::failbit | ios::badbit);
  fs.open(path.string().c_str(), ios::in | ios::out | ios::trunc);
}

cached_image::~cached_image() {
  flush_buffer();
  fs.close();
}

// cached_image
void cached_image::set_pixel(pos_t x, pos_t y, color& c)
{
  if (!(x < get_width())) { return; }
  if (!(y < get_height())) { return; }
  
  fs.seekp(get_offset(x, y));
  fs.write(reinterpret_cast<char*>(&c), sizeof(color));
}

void cached_image::get_pixel(pos_t x, pos_t y, color& c)
{
  if (!(x < get_width())) { return; }
  if (!(y < get_height())) { return; }
  
  fs.seekg(get_offset(x, y));
  fs.read(reinterpret_cast<char*>(&c), sizeof(color));
}

void cached_image::get_line(pos_t y, pos_t x, pos_t width, color* c)
{
  if (!(y < get_height())) { return; }
  if (!(x < get_width())) { return; }
  if (!(width + x < get_width())) { width = get_width() - x; }
  
  fs.seekg(get_offset(x, y));
  fs.read(reinterpret_cast<char*>(c), sizeof(color) * width);
}

void cached_image::set_line(pos_t y, pos_t x, pos_t width, color* c)
{
  if (!(y < get_height())) { return; }
  if (!(x < get_width())) { return; }
  if (!(width + x < get_width())) { width = get_width() - x; }
  
  fs.seekp(get_offset(x, y));
  fs.write(reinterpret_cast<char*>(c), sizeof(color) * width);
}

void cached_image::flush_buffer()
{
  if (buffer_set)
  {
    for (pos_t y = 0; y < buffer_h; y++) {
      set_line(buffer_y + y, buffer_x, buffer_w, &buffer[y * buffer_w]);
    }
  }
}

void cached_image::read_buffer()
{
  for (pos_t y = 0; y < buffer_h; y++) {
    get_line(buffer_y + y, buffer_x, buffer_w, &buffer[y * buffer_w]);
  }
}

#include <iostream>

void cached_image::blend_pixel(pos_t x, pos_t y, color &c)
{
  // do nothing if color is invisible
  if (c.is_invisible()) { return; }
  
  pos_t bx = x - buffer_x;
  pos_t by = y - buffer_y;
  pos_t bp = bx + by * buffer_w;

  if (!(bp < buffer_s)) {
    return;
  }
  
  buffer[bp].blend(c);
}


void cached_image::align(pos_t x, pos_t y, pos_t w, pos_t h)
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
