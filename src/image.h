// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _IMG_H_
#define _IMG_H_

#include "color.h"

#include <limits.h>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <string.h>
#include <vector>
#include <list>
#include <map>
#include <set>

#include <fstream>

struct image_operation {
  color c;
  int depth;
  uint16_t x, y;
};

struct image_op_key {
  int x, y;

  image_op_key(int x, int y) : x(x), y(y) {
  }
  
  bool operator<(const image_op_key& rhs) const {
    if (y < rhs.y) return true;
    if (y == rhs.y && x < rhs.x) return true;
    return false;
  }
};

//bool compare_image_operation(image_operation& lhs, image_operation& rhs);
/*{
}*/

class image_operations {
private:
  int maxx, maxy;
public:
  bool *lookup;
  int cache_hit_count, cache_miss_count;
  
  std::vector<image_operation> operations;
  
  //std::set<image_op_key> opaque_set;
  void add_pixel(int x, int y, color &c);

  void set_limits(int x, int y) {
    maxx = x;
    maxy = y;
    
    lookup = new bool[maxx * maxy];
    memset(lookup, 0x0, sizeof(bool) * maxx * maxy);
  }
  
  image_operations() : maxx(0), maxy(0), operations()
  {
    lookup = NULL;
  };
  
  ~image_operations() {
    if (lookup != NULL) {
      delete [] lookup;
    }
  }
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
  
  void fill(color& c);
  
  inline int get_width() { return w; };
  inline int get_height() { return h; };
  
  void composite(int xoffset, int yoffset, image_operations& oper);
  void composite(int xoffset, int yoffset, image_base& img);
  void safe_composite(int xoffset, int yoffset, image_base& img);
  
  inline size_t get_offset(int x, int y) {
    return (x * sizeof(color)) + (y * get_width() * sizeof(color));
  }
  
  bool save_png(const std::string filename, const char *title, progress_c);
  
  void safe_blend_pixel(int x, int y, color &c);
  virtual void blend_pixel(int x, int y, color &c) = 0;
  virtual void set_pixel(int x, int y, color& c) = 0;
  virtual void get_pixel(int x, int y, color& c) = 0;
  virtual void get_line(int y, color*) = 0;
};

class memory_image : public image_base {
private:
  uint8_t *colors;
public:
  memory_image(int w, int h) : image_base(w, h) {
    colors = new uint8_t[sizeof(color) * w * h];
    
    memset(colors, 0x0, sizeof(color) * w * h);
    
    /*for (int x = 0; x < get_width(); x++) {
      for (int y = 0; y < get_height(); y++) {
        set_pixel(x, y, blank);
      }
    }*/
  }
  
  ~memory_image() {
    delete [] colors;
  }

  void blend_pixel(int x, int y, color &c);
  void set_pixel(int x, int y, color&);
  void get_pixel(int x, int y, color&);
  void get_line(int y, color*);
};

#include <iostream>

struct icache {
  color c;
  int x, y;
  
  icache() : c(0x00, 0x00, 0x00, 0x00), x(-1), y(-1) {
  }
  
  inline bool isset() {
    return x >= 0 && y >= 0;
  }

  inline void unset() {
    x = -1;
    y = -1;
  }
};

class cached_image : public image_base {
private:
  static const size_t WRITE_SIZE = 4096 * 8;
  const char *path;
  icache *buffer;
  int buffer_size;
  std::fstream fs;
public:
  cached_image(const char *path, int w, int h, int buffer_size) :
    image_base(w, h),
    path(path),
    buffer_size(buffer_size)
  {
    using namespace std;
    
    fs.exceptions(ios::failbit | ios::badbit);
    fs.open(path, ios::in | ios::out | ios::binary | ios::trunc);
    
    streamsize total = get_width() * get_height() * COLOR_TYPE;
    streamsize write_size = WRITE_SIZE;
    
    uint8_t *nil = new uint8_t[write_size];
    memset(nil, 0x0, WRITE_SIZE);
    
    while (total > 0) {
      streamsize  write = min(total, write_size);
      fs.write(reinterpret_cast<char*>(nil), write);
      total -= write_size;
    }
    
    buffer = new icache[buffer_size];
    
    for (int i = 0; i < buffer_size; i++) {
      icache ic = buffer[i];
    }
    
    delete [] nil;
  }
  
  ~cached_image() {
    // flush the memory cache
    for (int i = 0; i < buffer_size; i++) { 
      icache* ic = &buffer[i];
      if (ic->isset()) {
        set_pixel(ic->x, ic->y, ic->c);
      }
    }
    
    delete [] buffer;
    fs.close();
  }
  
  void blend_pixel(int x, int y, color &c);
  void set_pixel(int x, int y, color&);
  void get_pixel(int x, int y, color&);
  void get_line(int y, color*);
};

#endif /* _IMG_H_ */
