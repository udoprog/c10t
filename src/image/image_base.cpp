// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/image_base.hpp"
#include "image/virtual_image.hpp"

#include <map>

#include <boost/numeric/conversion/cast.hpp>

// image_base
void image_base::fill(color &q)
{
  for (pos_t x = 0; x < get_width(); x++) {
    for (pos_t y = 0; y < get_height(); y++) {
      set_pixel(x, y, q);
    }
  }
}

void image_base::composite(int xoffset, int yoffset, boost::shared_ptr<image_operations> img)
{
  std::vector<image_operation>::size_type i = img->operations.size();
  
  align(xoffset, yoffset, img->maxx, img->maxy);
  
  while (i--) {
    image_operation op = img->operations[i];
    blend_pixel(xoffset + op.x, yoffset + op.y, op.c);
  }
}

void image_base::safe_blend_pixel(pos_t x, pos_t y, color &c)
{
  if (x >= w) return;
  if (y >= h) return;
  blend_pixel(x, y, c);
}

std::map<point2, image_base*> image_split(image_base* base, int pixels)
{
  std::map<point2, image_base*> map;
  
  for (image_base::pos_t w = 0, px = 0; w < base->get_width(); w += pixels, px++) {
    for (image_base::pos_t h = 0, py = 0; h < base->get_height(); h += pixels, py++) {
      point2 p(px, py);
      map[p] = new virtual_image(pixels, pixels, base, w, h);
    }
  }
  
  return map;
}
