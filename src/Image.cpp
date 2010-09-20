#include "Image.h"
#include "global.h"

#include <assert.h>

void MemoryImage::set_pixel_rgba(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  assert(x >= 0 && x < get_width());
  assert(y >= 0 && y < get_height());
  size_t p = get_offset(x, y);
  colors[p] = r;
  colors[p+1] = g;
  colors[p+2] = b;
  colors[p+3] = a;
}

void MemoryImage::get_pixel_rgba(int x, int y, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a){
  assert(x >= 0 && x < get_width());
  assert(y >= 0 && y < get_height());
  size_t p = get_offset(x, y);
  r = this->colors[p];
  g = this->colors[p+1];
  b = this->colors[p+2];
  a = this->colors[p+3];
}

void MemoryImage::blend_pixel(int x, int y, Color &c){
  Color o;
  get_pixel(x, y, o);

  if (o.is_transparent()) {
    set_pixel(x, y, c);
    return;
  }
  
  o.blend(c);
  set_pixel(x, y, o);
}

void Image::get_pixel(int x, int y, Color &c){
  get_pixel_rgba(x, y, c.r, c.g, c.b, c.a);
}

void Image::set_pixel(int x, int y, Color &q){
  set_pixel_rgba(x, y, q.r, q.g, q.b, q.a);
}

void Image::composite(int xoffset, int yoffset, ImageBuffer &img) {
  assert(xoffset >= 0);
  assert(xoffset + img.get_width() <= w);
  assert(yoffset >= 0);
  assert(yoffset + img.get_height() <= h);
  
  Color hp;
  
  for (int x = 0; x < img.get_width(); x++) {
    for (int y = 0; y < img.get_height(); y++) {
      Color base;
      get_pixel(xoffset + x, yoffset + y, base);
      
      if (img.reversed) {
        for (int h = img.get_pixel_depth(x, y); h >= 0; h--) {
          img.get_pixel(x, y, h, hp);
          base.blend(hp);
        }
      } else {
        for (int h = 0; h < img.get_pixel_depth(x, y); h++) {
          Color hp;
          img.get_pixel(x, y, h, hp);
          base.blend(hp);
        }
      }
      
      set_pixel(xoffset + x, yoffset + y, base);
    }
  }
}

void CachedImage::set_pixel_rgba(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  assert(x >= 0 && x < get_width());
  assert(y >= 0 && y < get_height());
  assert(f != NULL);
  size_t p = get_offset(x, y);
  uint8_t cb[] = {r, g, b, a};
  fseek(f, p, SEEK_SET);
  assert(fwrite(cb, sizeof(cb), 1, f) == 1);
}

void CachedImage::get_pixel_rgba(int x, int y, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a) {
  assert(x >= 0 && x < get_width());
  assert(y >= 0 && y < get_height());
  assert(f != NULL);
  size_t p = get_offset(x, y);
  uint8_t cb[] = {0x00, 0x00, 0x00, 0x00};
  fseek(f, p, SEEK_SET);
  assert(fread(cb, sizeof(cb), 1, f) == 1);
  r = cb[0];
  g = cb[1];
  b = cb[2];
  a = cb[3];
}

void CachedImage::blend_pixel(int x, int y, Color &c){
  size_t s = (x + y * get_width()) % buffer_size;
  
  icache ic = buffer[s];
  
  // cache hit
  if (ic.is_set) {
    // cache hit, but wrong coordinatesflush pizel to file
    if (ic.x != x || ic.y != y)  {
      set_pixel(ic.x, ic.y, ic.c);
      ic.c = c;
      return;
    }
  }
  else {
    ic.c = c;
    ic.is_set = true;
  }
  
  if (ic.c.is_transparent()) {
    return;
  }
  
  ic.c.blend(c);
}

void ImageBuffer::set_pixel_rgba(int x, int y, int z, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  assert(x >= 0 && x < w);
  assert(y >= 0 && y < h);
  assert(z >= 0 && z < d);
  size_t p = (z * COLOR_TYPE) + (y * d * COLOR_TYPE) + (x * d * h * COLOR_TYPE);
  colors[p] = r;
  colors[p+1] = g;
  colors[p+2] = b;
  colors[p+3] = a;
}

void ImageBuffer::set_pixel(int x, int y, int z, Color &q){
  set_pixel_rgba(x, y, z, q.r, q.g, q.b, q.a);
}

void ImageBuffer::get_pixel(int x, int y, int z, Color &c) {
  assert(x >= 0 && x < w);
  assert(y >= 0 && y < h);
  assert(z >= 0 && z < d);
  size_t p = (z * COLOR_TYPE) + (y * d * COLOR_TYPE) + (x * d * h * COLOR_TYPE);
  c.r = colors[p];
  c.g = colors[p+1];
  c.b = colors[p+2];
  c.a = colors[p+3];
}

void ImageBuffer::add_pixel(int x, int y, Color &c) {
  if (c.is_opaque()) {
    set_pixel_depth(x, y, 0);
  }
  
  add_pixel(x, y, c.r, c.g, c.b, c.a);
}

void ImageBuffer::add_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  uint8_t ph = get_pixel_depth(x, y);
  set_pixel_rgba(x, y, ph, r, g, b, a);
  set_pixel_depth(x, y, ph + 1);
}

void ImageBuffer::set_reversed(bool reversed) {
  this->reversed = reversed;
}

void ImageBuffer::set_pixel_depth(int x, int y, uint8_t ph) {
  assert(x >= 0 && x < w);
  assert(y >= 0 && y < h);
  heights[x + (y * w)] = ph;
}

uint8_t ImageBuffer::get_pixel_depth(int x, int y) {
  assert(x >= 0 && x < w);
  assert(y >= 0 && y < h);
  return heights[x + (y * w)];
}
