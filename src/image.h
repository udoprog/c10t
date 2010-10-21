// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _IMG_H_
#define _IMG_H_

#include "2d/cube.h"
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

#include <boost/ptr_container/ptr_map.hpp>

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

class virtual_image;

class image_base {
protected:
  size_t w, h;
public:
  typedef void (*progress_c)(int , int);
  
  static const short COLOR_TYPE = 4;
  
  image_base(size_t w, size_t h) : w(w), h(h) {
  }
  
  virtual ~image_base() {
  }
  
  void fill(color& c);
  
  inline size_t get_width() { return w; };
  inline size_t get_height() { return h; };
  
  void composite(int xoffset, int yoffset, image_operations& oper);
  void composite(int xoffset, int yoffset, image_base& img);
  void safe_composite(int xoffset, int yoffset, image_base& img);
  
  inline size_t get_offset(int x, int y) {
    return (x * sizeof(color)) + (y * get_width() * sizeof(color));
  }
  
  bool save_png(const std::string filename, const char *title, progress_c);
  
  void safe_blend_pixel(size_t x, size_t y, color &c);

  void get_line(size_t y, color *c){
    get_line(y, 0, get_width(), c);
  }
  
  virtual void blend_pixel(size_t x, size_t y, color &c) = 0;
  virtual void set_pixel(size_t x, size_t y, color& c) = 0;
  virtual void get_pixel(size_t x, size_t y, color& c) = 0;
  virtual void get_line(size_t y, size_t offset, size_t w, color*) = 0;
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

  void blend_pixel(size_t x, size_t y, color &c);
  void set_pixel(size_t x, size_t y, color&);
  void get_pixel(size_t x, size_t y, color&);
  void get_line(size_t y, size_t offset, size_t width, color*);
};

class virtual_image : public image_base {
private:
  image_base* base;
  size_t x, y;
public:
  virtual_image(size_t w, size_t h, image_base* base, size_t x, size_t y) : image_base(w, h), base(base), x(x), y(y)
  {
  }
  
  void blend_pixel(size_t x, size_t y, color &c) {
    base->blend_pixel(this->x + x, this->y + y, c);
  }
  
  void set_pixel(size_t x, size_t y, color& c) {
    base->set_pixel(this->x + x, this->y + y, c);
  }
  
  void get_pixel(size_t x, size_t y, color& c) {
    base->get_pixel(this->x + x, this->y + y, c);
  }
  
  void get_line(size_t y, size_t x, size_t width, color* c) {
    size_t o_x = this->x + x;
    size_t o_y = this->y + y;
    size_t p_width = 0;
    
    if (o_x > base->get_width()) { goto exit_zero; }
    if (o_y > base->get_height()) { goto exit_zero; }
    
    p_width = std::min(base->get_width() - o_x, width);
    
    if (p_width == width) {
      base->get_line(o_y, o_x, width, c);
    }
    else {
      base->get_line(o_y, o_x, p_width, c);
      // fill up the rest with zeroes
      memset(c + p_width, 0x0, sizeof(color) * (width - p_width));
    }

    return;

exit_zero:
    memset(c, 0x0, sizeof(color) * width);
  }
};

#include <iostream>

struct icache {
  color c;
  size_t x;
  size_t y;
  bool isset;
  
  icache() : c(0x00, 0x00, 0x00, 0x00), isset(false) {
  }
  
  inline bool is_set() {
    return isset;
  }
  
  inline void unset() {
    isset = false;
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
  cached_image(const char *path, size_t w, size_t h, size_t buffer_size) :
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
    
    for (size_t i = 0; i < buffer_size; i++) {
      icache ic = buffer[i];
    }
    
    delete [] nil;
  }
  
  ~cached_image() {
    // flush the memory cache
    for (int i = 0; i < buffer_size; i++) { 
      icache* ic = &buffer[i];
      if (ic->is_set()) {
        set_pixel(ic->x, ic->y, ic->c);
      }
    }
    
    delete [] buffer;
    fs.close();
  }
  
  void blend_pixel(size_t x, size_t y, color &c);
  void set_pixel(size_t x, size_t y, color&);
  void get_pixel(size_t x, size_t y, color&);
  void get_line(size_t y, size_t offset, size_t width, color*);
};

std::map<point2, image_base*> image_split(image_base* base, int pixels);

#endif /* _IMG_H_ */
