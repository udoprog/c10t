// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/image_base.hpp"
#include "image/virtual_image.hpp"

#include <map>

#include <boost/numeric/conversion/cast.hpp>

// image_base
void image_base::fill(color &q)
{
  for (size_t x = 0; x < get_width(); x++) {
    for (size_t y = 0; y < get_height(); y++) {
      set_pixel(x, y, q);
    }
  }
}

void image_base::composite(int xoffset, int yoffset, boost::shared_ptr<image_operations> img)
{
  if (!(xoffset >= 0)) { return; }
  if (!(yoffset >= 0)) { return; }
  
  std::vector<image_operation>::size_type i = img->operations.size();
  
  align(xoffset, yoffset, img->maxx, img->maxy);
  
  while (i--) {
    image_operation op = img->operations[i];
    blend_pixel(xoffset + op.x, yoffset + op.y, op.c);
    //base.blend(op.c);
    //set_pixel(xoffset + op.x, yoffset + op.y, base);
  }
}

void image_base::composite(int xoffset, int yoffset, image_base &img)
{
  if (!(xoffset >= 0)) { return; }
  if (!(yoffset >= 0)) { return; }

  size_t s_xoffset = boost::numeric_cast<size_t>(xoffset);
  size_t s_yoffset = boost::numeric_cast<size_t>(yoffset);
  
  if (!(s_xoffset + img.get_width() <= w)) { return; }
  if (!(s_yoffset + img.get_height() <= h)) { return; }
  
  color hp;
  color base;
  
  for (size_t x = 0; x < img.get_width(); x++) {
    for (size_t y = 0; y < img.get_height(); y++) {
      get_pixel(xoffset + x, s_yoffset + y, base);
      img.get_pixel(x, y, hp);
      base.blend(hp);
      set_pixel(xoffset + x, s_yoffset + y, base);
    }
  }
}

void image_base::safe_composite(int xoffset, int yoffset, image_base &img)
{
  if (xoffset < 0) return;
  if (xoffset + img.get_width() > w) return;
  if (yoffset < 0) return;
  if (yoffset + img.get_height() > h) return;
  composite(xoffset, yoffset, img);
}

void image_base::safe_blend_pixel(size_t x, size_t y, color &c)
{
  if (x >= w) return;
  if (y >= h) return;
  blend_pixel(x, y, c);
}

std::map<point2, image_base*> image_split(image_base* base, int pixels)
{
  std::map<point2, image_base*> map;
  
  for (size_t w = 0, px = 0; w < base->get_width(); w += pixels, px++) {
    for (size_t h = 0, py = 0; h < base->get_height(); h += pixels, py++) {
      point2 p(px, py);
      map[p] = new virtual_image(pixels, pixels, base, w, h);
    }
  }
  
  return map;
}
