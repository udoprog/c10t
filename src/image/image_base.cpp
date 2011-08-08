// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/image_base.hpp"

#include <cstring>

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

void image_base::clear() {
  boost::shared_array<color> line(new color[get_width()]);
  ::memset(line.get(), 0x0, get_width() * sizeof(color));

  for (pos_t y = 0; y < get_height(); y++)
    set_line(y, 0, get_width(), line.get());
}

void image_base::composite(int x, int y, image_operations_ptr opers)
{
  std::vector<image_operation>::size_type i = opers->operations.size();
  
  align(x, y, opers->max_x, opers->max_y);
  
  while (i--)
  {
    image_operation op = opers->operations[i];
    blend_pixel(x + op.x, y + op.y, op.c);
  }
}

void image_base::drawLine(pos_t x1, pos_t y1, pos_t x2, pos_t y2, color &c)
{
    int sx, sy;
    int dx = abs(x2-x1);
    int dy = abs(y2-y1);
    sx = (x1<x2) ? 1 : -1 ;
    sy = (y1<y2) ? 1 : -1 ;
    int err = dx-dy;

    do
    {
        this->set_pixel(x1, y1, c);
        int e2 = 2*err;
        if(e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if(e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    } while(x1 != x2 || y1 != y2);
}

void image_base::safe_blend_pixel(pos_t x, pos_t y, color &c)
{
  if (x >= w) return;
  if (y >= h) return;
  blend_pixel(x, y, c);
}

void image_base::resize(image_ptr target) {
  if (target->get_width() > get_width()) {
    // scale up
    unsigned int factor = target->get_width() / get_width();
    boost::shared_array<color> line(new color[get_width()]);

    unsigned int scanline = 0;
    bool first = false;

    for (pos_t y = 0; y < target->get_height(); y++) {
      pos_t y_p = (y / factor);

      if (y_p != scanline || !first) {
        get_line(y_p, pos_t(0), get_width(), line.get());
        first = true;
        scanline = y_p;
      }

      for (pos_t x = 0; x < target->get_width(); x++) {
        pos_t x_p = (x / factor);
        target->set_pixel(x, y, line[x_p]);
      }
    }
  }
  else {
    // scale down
    unsigned int factor = get_width() / target->get_width();

    boost::shared_array<color> line(new color[get_width()]);

    for (pos_t y = 0; y < target->get_height(); y++) {
      pos_t y_p = ((y * factor) + factor / 2);
      get_line(y_p, pos_t(0), get_width(), line.get());
      
      for (pos_t x = 0; x < target->get_width(); x++) {
        pos_t x_p = ((x * factor) + factor / 2);
        target->set_pixel(x, y, line[x_p]);
      }
    }
  }
}
