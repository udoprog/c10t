// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef CACHED_IMAGE
#define CACHED_IMAGE

#include <fstream>
#include <string.h>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/scoped_array.hpp>

#include "image/image_base.hpp"
#include "image/color.hpp"

class cached_image : public image_base {
private:
  static const size_t WRITE_SIZE = 4096 * 8;
  const char *path;
  std::fstream fs;

  size_t l_total;
  color* buffer;
  bool buffer_set;
  size_t buffer_s;
  size_t buffer_w;
  size_t buffer_h;
  size_t buffer_x;
  size_t buffer_y;
public:
  cached_image(const char *path, size_t w, size_t h, size_t memory_limit, size_t l_w, size_t l_h) :
    image_base(w, h),
    path(path)
  {
    using namespace std;
    
    fs.exceptions(ios::failbit | ios::badbit);
    fs.open(path, ios::in | ios::out | ios::binary | ios::trunc);
    
    streamsize total =
      boost::numeric_cast<streamsize>(get_width()) *
      boost::numeric_cast<streamsize>(get_height()) *
      COLOR_TYPE;
    
    streamsize write_size = WRITE_SIZE;
    
    uint8_t *nil = new uint8_t[write_size];
    memset(nil, 0x0, WRITE_SIZE);
    
    while (total > 0) {
      streamsize  write = min(total, write_size);
      fs.write(reinterpret_cast<char*>(nil), write);
      total -= write_size;
    }
    
    delete [] nil;
    
    buffer_s = memory_limit / sizeof(color);
    buffer_set = false;
    buffer = new color[buffer_s];
  }
  
  ~cached_image() {
    flush_buffer();
    fs.close();
  }
  
  void read_buffer();
  void flush_buffer();
  
  void blend_pixel(size_t x, size_t y, color &c);
  void set_pixel(size_t x, size_t y, color&);
  void get_pixel(size_t x, size_t y, color&);
  void get_line(size_t y, size_t offset, size_t width, color*);
  void set_line(size_t y, size_t offset, size_t width, color*);
  void align(size_t x, size_t y, size_t w, size_t h);
};

#endif /* CACHED_IMAGE */
