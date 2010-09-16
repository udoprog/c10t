#ifndef _IMG_H_
#define _IMG_H_

#include "Color.h"

class Image {
private:
  static const short COLOR_TYPE = 4;
  uint8_t *colors;
  int w, h;
public:
  Image(int w, int h) : w(w), h(h) {
    colors = new uint8_t[COLOR_TYPE * w * h];

    for (int x = 0; x < w; x++) {
      for (int y = 0; y < h; y++) {
        set_pixel(x, y, 0xff, 0xff, 0xff, 0x00);
      }
    }
  }

  ~Image() {
    delete [] colors;
  }

  void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  
  void set_pixel(int x, int y, Color &c);
  void blend_pixel(int x, int y, Color &c);
  
  void get_pixel(int x, int y, Color &c);
  
  int get_width();
  int get_height();
  
  void composite(int xoffset, int yoffset, Image &img);
};

class ImageBuffer {
  static const short COLOR_TYPE = 4;
  uint8_t *colors;
  uint16_t *heights;
  
  int w;
  int h;
  int d;
  
  ImageBuffer(int w, int h, int z) : w(w), h(h), d(d) {
    colors = new uint8_t[COLOR_TYPE * w * h * z];
    heights = new uint16_t[w * h];
    
    for (int x = 0; x < w; x++) {
      for (int y = 0; y < h; y++) {
        for (int z = 0; z < d; z++) {
          set_pixel(x, y, z, 0xff, 0xff, 0xff, 0x00);
        }
        
        set_height(x, y, 0);
      }
    }
  }
  
  void add_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  void add_pixel(int x, int y, Color &c);
  void set_pixel(int x, int y, int z, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  void set_height(int x, int y, uint16_t h);
  uint16_t get_height(int x, int y);
};

#endif /* _IMG_H_ */
