// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image.h"
#include "global.h"

#include <algorithm>
#include <assert.h>
#include <stdint.h>
#include <limits>

#include <png.h>

void image_operations::add_pixel(int x, int y, color &c) {
  assert(x >= 0);
  assert(y >= 0);
  assert(x < std::numeric_limits<uint16_t>::max());
  assert(y < std::numeric_limits<uint16_t>::max());
  
  if (c.is_invisible()) {
    return;
  }
  
  image_operation oper;
  
  maxx = std::max(maxx, x);
  maxy = std::max(maxy, y);
  
  oper.x = (uint16_t)x;
  oper.y = (uint16_t)y;
  oper.order = ++order;
  oper.c = c;
  
  operations.push_back(oper);
}

void memory_image::set_pixel(int x, int y, color &c) {
  assert(x >= 0 && x < get_width());
  assert(y >= 0 && y < get_height());
  c.write(this->colors + get_offset(x, y));
}

void memory_image::get_pixel(int x, int y, color &c){
  assert(x >= 0 && x < get_width());
  assert(y >= 0 && y < get_height());
  c.read(this->colors + get_offset(x, y));
}

void memory_image::get_line(int y, color *c){
  assert(y >= 0 && y < get_height());
  memcpy(c, this->colors + get_offset(0, y), get_width() * sizeof(color));
}

void memory_image::blend_pixel(int x, int y, color &c){
  color o;
  get_pixel(x, y, o);

  if (o.is_invisible()) {
    set_pixel(x, y, c);
    return;
  }
  
  o.blend(c);
  set_pixel(x, y, o);
}

void image_base::fill(color &q){
  for (int x = 0; x < get_width(); x++) {
    for (int y = 0; y < get_height(); y++) {
      set_pixel(x, y, q);
    }
  }
}

void image_base::composite(int xoffset, int yoffset, image_operations &img) {
  assert(xoffset >= 0);
  assert(yoffset >= 0);
  
  color hp;
  
  for (std::vector<image_operation>::iterator it = img.operations.begin();
      it != img.operations.end(); it++) {
    image_operation op = *it;

    color base;
    get_pixel(xoffset + op.x, yoffset + op.y, base);
    base.blend(op.c);
    set_pixel(xoffset + op.x, yoffset + op.y, base);
  }
}

void image_base::composite(int xoffset, int yoffset, image_base &img) {
  assert(xoffset >= 0);
  assert(xoffset + img.get_width() <= w);
  assert(yoffset >= 0);
  assert(yoffset + img.get_height() <= h);

  color hp;
  color base;

  for (int x = 0; x < img.get_width(); x++) {
    for (int y = 0; y < img.get_height(); y++) {
      get_pixel(xoffset + x, yoffset + y, base);
      img.get_pixel(x, y, hp);
      base.blend(hp);
      set_pixel(xoffset + x, yoffset + y, base);
    }
  }
}

void image_base::safe_composite(int xoffset, int yoffset, image_base &img) {
  if (xoffset < 0) return;
  if (xoffset + img.get_width() > w) return;
  if (yoffset < 0) return;
  if (yoffset + img.get_height() > h) return;
  composite(xoffset, yoffset, img);
}

void image_base::safe_blend_pixel(int x, int y, color &c) {
  if (x < 0 || x >= w) return;
  if (y < 0 ||y >= h) return;
  blend_pixel(x, y, c);
}

bool image_base::save_png(const std::string path, const char *title, progress_c progress_c_cb)
{
  bool ret = true;
  
  FILE *fp;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  png_bytep row = NULL;
  
  if (path.compare("-") == 0) {
    fp = stdout;
  } else {
    fp = fopen(path.c_str(), "wb");
    
    if (fp == NULL) {
       ret = false;
       goto finalise;
    }
  }
  
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
 
  if (png_ptr == NULL) {
     ret = false;
     goto finalise;
  }

  info_ptr = png_create_info_struct(png_ptr);

  if (info_ptr == NULL) {
     ret = false;
     goto finalise;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
     ret = false;
     goto finalise;
  }

  png_init_io(png_ptr, fp);

  png_set_IHDR(png_ptr, info_ptr, get_width(), get_height(),
        8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
   
  if (title != NULL) {
     png_text title_text;
     title_text.compression = PNG_TEXT_COMPRESSION_NONE;
     title_text.key = (char *)"Title";
     title_text.text = (char *)title;
     png_set_text(png_ptr, info_ptr, &title_text, 1);
  }

  png_write_info(png_ptr, info_ptr);

  row = (png_bytep) malloc(4 * get_width() * sizeof(png_byte));
  
  int y;
  
  for (y=0 ; y < get_height(); y++) {
    if (progress_c_cb != NULL) progress_c_cb(y, get_height());
    
    get_line(y, reinterpret_cast<color*>(row));
    
    png_write_row(png_ptr, row);
  }
  
  if (progress_c_cb != NULL) progress_c_cb(get_height(), get_height());
  
  png_write_end(png_ptr, NULL);

finalise:
  if (fp != NULL) {
    fclose(fp);
  }
   
  if (info_ptr != NULL) {
    png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
  }

  if (png_ptr != NULL) {
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
  }

  if (row != NULL) {
    free(row);
  }
  
  return ret;
}


void cached_image::set_pixel(int x, int y, color& c) {
  assert(x >= 0 && x < get_width());
  assert(y >= 0 && y < get_height());
  fs.seekp(get_offset(x, y), std::ios::beg);
  fs.write(reinterpret_cast<char*>(&c), sizeof(color));
}

void cached_image::get_pixel(int x, int y, color& c) {
  assert(x >= 0 && x < get_width());
  assert(y >= 0 && y < get_height());
  fs.seekg(get_offset(x, y), std::ios::beg);
  fs.read(reinterpret_cast<char*>(&c), sizeof(color));
}

void cached_image::get_line(int y, color *c){
  assert(y >= 0 && y < get_height());
  fs.seekg(get_offset(0, y), std::ios::beg);
  fs.read(reinterpret_cast<char*>(c), sizeof(color) * get_width());
}

void cached_image::blend_pixel(int x, int y, color &c){
  // do nothing if color is invisible
  if (c.is_invisible()) {
    return;
  }
  
  size_t s = (x + y * get_width()) % buffer_size;
  
  icache* ic = &buffer[s];
  
  // cache hit
  if (ic->isset()) {
    // cache hit, but wrong coordinates - flush pixel to file
    if (ic->x != x || ic->y != y)  {
      set_pixel(ic->x, ic->y, ic->c);
      ic->c = c;
      ic->x = x;
      ic->y = y;
      return;
    }
    
    ic->c.blend(c);
  }
  // cache miss - just set the cache
  else {
    ic->c = c;
    ic->x = x;
    ic->y = y;
  }
}

/* align the memory cache with the actual image */
/*void cached_image::align(int xp, int yp, int xw, int yw)
{
  for (int y = yp; y < yp + yw; y++) {
    for (int x = xp; x < xp + xw; x++) {
      size_t s = (x + y * get_width()) % buffer_size;
      icache ic = buffer[s];
    }
  }
}
*/
