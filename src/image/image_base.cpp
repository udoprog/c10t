// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/image_base.hpp"
#include "image/virtual_image.hpp"

#include <map>

#include <boost/numeric/conversion/cast.hpp>

#include <png.h>

// image_base
void image_base::fill(color &q)
{
  for (size_t x = 0; x < get_width(); x++) {
    for (size_t y = 0; y < get_height(); y++) {
      set_pixel(x, y, q);
    }
  }
}

void image_base::composite(int xoffset, int yoffset, image_operations &img)
{
  std::vector<image_operation>::size_type i = img.operations.size();
  
  while (i--) {
    image_operation op = img.operations[i];
    
    color base;
    get_pixel(xoffset + op.x, yoffset + op.y, base);
    base.blend(op.c);
    set_pixel(xoffset + op.x, yoffset + op.y, base);
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
  
  for (size_t y = 0; y < get_height(); y++) {
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

std::string get_png_version()
{
  return png_get_copyright(NULL);
}
