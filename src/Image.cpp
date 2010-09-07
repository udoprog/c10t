#include "Image.h"
#include "global.h"

#include <assert.h>

void Image::SetPixel(int x, int y, Color q){
  assert(x >= 0 && x < w);
  assert(y >= 0 && y < h);
  this->d[x+y*w] = q;
}

Color Image::GetPixel(int x,int y){
  assert(x >= 0 && x < w);
  assert(y >= 0 && y < h);
  return this->d[x+y*w];
}

Color *Image::GetPPointer(int x, int y){
  assert(x >= 0 && x < w);
  assert(y >= 0 && y < h);
  return &this->d[x+y*w];
}
