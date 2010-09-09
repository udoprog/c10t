#include "Image.h"
#include "global.h"

#include <assert.h>

void Image::set_pixel(int x, int y, Color q){
  assert(x >= 0 && x < w);
  assert(y >= 0 && y < h);
  this->colors[x+y*w] = q;
}

Color Image::get_pixel(int x,int y){
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
