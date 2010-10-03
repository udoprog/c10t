// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _IMG_H_
#define _IMG_H_

#include "color.h"

#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <string.h>

class image_buffer {
public:
  static const short COLOR_TYPE = 4;
  uint8_t *colors;
  uint8_t *heights;
  
  int w;
  int h;
  int d;
  bool reversed;
  
  image_buffer(int w, int h, int d) : w(w), h(h), d(d), reversed(false) {
    colors = new uint8_t[COLOR_TYPE * w * h * d];
    heights = new uint8_t[w * h];
    
    for (int x = 0; x < w; x++) {
      for (int y = 0; y < h; y++) {
        set_pixel_rgba(x, y, 0, 0xff, 0xff, 0xff, 0x00);
        set_pixel_depth(x, y, 0);
      }
    }
  }

  ~image_buffer() {
    delete [] colors;
    delete [] heights;
  }

  int get_width() { return w; };
  int get_height() { return h; };
  int get_depth() { return d; };
  
  void add_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  void add_pixel(int x, int y, color &c);
  void set_pixel_rgba(int x, int y, int z, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  void set_pixel(int x, int y, int z, color &c);
  
  void set_reversed(bool);
  void set_pixel_depth(int x, int y, uint8_t h);
  uint8_t get_pixel_depth(int x, int y);

  void get_pixel(int x, int y, int z, color &c);
};

class image_base {
private:
  int w, h;
public:
  typedef void (*progress_c)(int , int);
  
  static const short COLOR_TYPE = 4;
  
  image_base(int w, int h) : w(w), h(h) {
  }

  virtual ~image_base() {
  }
  
  void set_pixel(int x, int y, color &c);
  void get_pixel(int x, int y, color &c);
  void fill(color& c);
  
  inline int get_width() { return w; };
  inline int get_height() { return h; };
  
  void composite(int xoffset, int yoffset, image_buffer& img);
  void composite(int xoffset, int yoffset, image_base& img);
  void safe_composite(int xoffset, int yoffset, image_base& img);
  
  inline size_t get_offset(int x, int y) {
    return (x * COLOR_TYPE) + (y * get_width() * COLOR_TYPE);
  }
  
  bool save_png(const char *filename, const char *title, progress_c);
  
  void safe_blend_pixel(int x, int y, color &c);
  virtual void blend_pixel(int x, int y, color &c) = 0;
  virtual void set_pixel_rgba(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
  virtual void get_pixel_rgba(int x, int y, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a) = 0;
};

class memory_image : public image_base {
private:
  uint8_t *colors;
public:
  memory_image(int w, int h) : image_base(w, h) {
    colors = new uint8_t[COLOR_TYPE * w * h];
    
    for (int x = 0; x < get_width(); x++) {
      for (int y = 0; y < get_height(); y++) {
        set_pixel_rgba(x, y, 0xff, 0xff, 0xff, 0x00);
      }
    }
  }

  ~memory_image() {
    delete [] colors;
  }

  void blend_pixel(int x, int y, color &c);
  void set_pixel_rgba(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  void get_pixel_rgba(int x, int y, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a);
};

#include <iostream>

struct icache {
  color c;
  int x, y;
  bool is_set;
  icache() : c(0x00, 0x00, 0x00, 0x00), x(0), y(0), is_set(false) {}
};

class cached_image : public image_base {
private:
  static const size_t WRITE_SIZE = 4096 * 8;
  FILE *f;
  const char *path;
  icache *buffer;
  int buffer_size;
public:
  cached_image(const char *path, int w, int h, int buffer_size) : image_base(w, h), path(path), buffer_size(buffer_size) {
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
    
    delete [] nil;
  }
  
  ~cached_image() {
    if (f != NULL) {
      fclose(f);

      // flush the memory cache
      for (int i = 0; i < buffer_size; i++) { 
        icache ic = buffer[i];
        if (ic.is_set) {
          set_pixel(ic.x, ic.y, ic.c);
        }
      }
      
      delete [] buffer;
    }
  }
  
  void blend_pixel(int x, int y, color &c);
  void set_pixel_rgba(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  void get_pixel_rgba(int x, int y, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a);
};

#endif /* _IMG_H_ */
