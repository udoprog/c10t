#ifndef _IMG_H_
#define _IMG_H_

#include "Color.h"

#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <string.h>

class ImageBuffer {
public:
  static const short COLOR_TYPE = 4;
  uint8_t *colors;
  uint8_t *heights;
  
  int w;
  int h;
  int d;
  bool reversed;
  
  ImageBuffer(int w, int h, int d) : w(w), h(h), d(d), reversed(false) {
    colors = new uint8_t[COLOR_TYPE * w * h * d];
    heights = new uint8_t[w * h];
    
    for (int x = 0; x < w; x++) {
      for (int y = 0; y < h; y++) {
        set_pixel_rgba(x, y, 0, 0xff, 0xff, 0xff, 0x00);
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
  void set_pixel_rgba(int x, int y, int z, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  void set_pixel(int x, int y, int z, Color &c);
  
  void set_reversed(bool);
  void set_pixel_depth(int x, int y, uint8_t h);
  uint8_t get_pixel_depth(int x, int y);

  void get_pixel(int x, int y, int z, Color &c);
};

class Image {
private:
  int w, h;
public:
  static const short COLOR_TYPE = 4;
  
  Image(int w, int h) : w(w), h(h) {
  }

  virtual ~Image() {
  }
  
  void set_pixel(int x, int y, Color &c);
  void get_pixel(int x, int y, Color &c);
  
  inline int get_width() { return w; };
  inline int get_height() { return h; };
  
  void composite(int xoffset, int yoffset, ImageBuffer &img);
  
  inline size_t get_offset(int x, int y) {
    return (x * COLOR_TYPE) + (y * get_width() * COLOR_TYPE);
  }
  
  virtual void blend_pixel(int x, int y, Color &c) = 0;
  virtual void set_pixel_rgba(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
  virtual void get_pixel_rgba(int x, int y, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a) = 0;
};

class MemoryImage : public Image {
private:
  uint8_t *colors;
public:
  MemoryImage(int w, int h) : Image(w, h) {
    colors = new uint8_t[COLOR_TYPE * w * h];
    
    for (int x = 0; x < get_width(); x++) {
      for (int y = 0; y < get_height(); y++) {
        set_pixel_rgba(x, y, 0xff, 0xff, 0xff, 0x00);
      }
    }
  }

  ~MemoryImage() {
    delete [] colors;
  }

  void blend_pixel(int x, int y, Color &c);
  void set_pixel_rgba(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  void get_pixel_rgba(int x, int y, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a);
};

#include <iostream>

struct icache {
  Color c;
  int x, y;
  bool is_set;
};

class CachedImage : public Image {
private:
  static const size_t WRITE_SIZE = 4096 * 8;
  FILE *f;
  const char *path;
  icache *buffer;
  int buffer_size;
public:
  CachedImage(const char *path, int w, int h, int buffer_size) : Image(w, h), path(path), buffer_size(buffer_size) {
    size_t total = get_width() * get_height() * COLOR_TYPE;
    size_t write_size = WRITE_SIZE;
    uint8_t *nil = new uint8_t[write_size];
    memset(nil, 0x0, WRITE_SIZE);
    
    f = fopen(path, "w+b");
    assert(f != NULL);
    
    while (total > 0) {
      size_t write = std::min(total, write_size);
      int r;
      assert((r = fwrite(nil, 1, write, f)) > 0);
      total -= r;
    }
    
    delete nil;
    
    buffer = new icache[buffer_size];

    for (int i = 0; i < buffer_size; i++) {
      icache ic = buffer[i];
      ic.c.r = 0x0;
      ic.c.g = 0x0;
      ic.c.b = 0x0;
      ic.c.a = 0x0;
      ic.is_set = false;
      ic.x = 0;
      ic.y = 0;
    }
  }
  
  ~CachedImage() {
    if (f != NULL) {
      fclose(f);

      for (int i = 0; i < buffer_size; i++) { 
        icache ic = buffer[i];
        if (ic.is_set) {
          set_pixel(ic.x, ic.y, ic.c);
          ic.is_set = false;
        }
      }
      
      delete [] buffer;
    }
  }
  
  void blend_pixel(int x, int y, Color &c);
  void set_pixel_rgba(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  void get_pixel_rgba(int x, int y, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a);
};

#endif /* _IMG_H_ */
