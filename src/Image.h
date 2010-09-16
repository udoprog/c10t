#ifndef _IMG_H_
#define _IMG_H_

#include "Color.h"

class ImageBuffer {
public:
  static const short COLOR_TYPE = 4;
  uint8_t *colors;
  uint16_t *heights;
  
  int w;
  int h;
  int d;
  
  ImageBuffer(int w, int h, int d) : w(w), h(h), d(d) {
    colors = new uint8_t[COLOR_TYPE * w * h * d];
    heights = new uint16_t[w * h];
    
    for (int x = 0; x < w; x++) {
      for (int y = 0; y < h; y++) {
        set_pixel(x, y, 0, 0xff, 0xff, 0xff, 0x00);
        set_pixel_depth(x, y, 0);
      }
    }
  }

  ~ImageBuffer() {
    delete [] colors;
    delete [] heights;
  }

  int get_width() { return w; };
  int get_height() { return h; };
  int get_depth() { return d; };
  
  void add_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  void add_pixel(int x, int y, Color &c);
  void set_pixel(int x, int y, int z, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  void set_pixel(int x, int y, int z, Color &c);
  
  void set_pixel_depth(int x, int y, uint16_t h);
  uint16_t get_pixel_depth(int x, int y);

  void get_pixel(int x, int y, int z, Color &c);
};


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
  
  void composite(int xoffset, int yoffset, ImageBuffer &img);
};

#endif /* _IMG_H_ */
