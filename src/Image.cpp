#include "Image.h"
#include "global.h"

#include <assert.h>

void Image::set_pixel(int x, int y, Color q){
  assert(x >= 0 && x < w);
  assert(y >= 0 && y < h);
  this->colors[x+y*w] = q;
}

Color Image::get_pixel(int x, int y){
  assert(x >= 0 && x < w);
  assert(y >= 0 && y < h);
  return this->colors[x+y*w];
}

int Image::get_width() {
  return w;
}

int Image::get_height() {
  return h;
}

void Image::composite(int xoffset, int yoffset, Image &img) {
  assert(xoffset >= 0);
  assert(xoffset + img.get_width() <= w);
  assert(yoffset >= 0);
  assert(yoffset + img.get_height() <= h);
  
  for (int x = 0; x < img.get_width(); x++) {
    for (int y = 0; y < img.get_height(); y++) {
      set_pixel(xoffset + x, yoffset + y, img.get_pixel(x, y));
    }
  }
}
