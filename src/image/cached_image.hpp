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
#include "algorithm.hpp"

class cached_image : public image_base {
private:
  static const pos_t WRITE_SIZE = 4096 * 8;
  const char *path;
  std::fstream fs;

  pos_t l_total;
  pos_t buffer_s;
  bool buffer_set;
  boost::shared_array<color> buffer;
  
  pos_t buffer_w;
  pos_t buffer_h;
  pos_t buffer_x;
  pos_t buffer_y;
  
  void read_buffer();
  void flush_buffer();
public:
  cached_image(const char *path, pos_t w, pos_t h, pos_t l_w, pos_t l_h, nonstd::reporting<std::streamsize>& reporter) :
    image_base(w, h),
    path(path),
    buffer_s((l_w + 1) * l_h),
    buffer_set(false),
    buffer(new color[buffer_s])
  {
    using namespace std;
    
    fs.exceptions(ios::failbit | ios::badbit);
    fs.open(path, ios::in | ios::out | ios::binary | ios::trunc);
    
    streamsize total =
      boost::numeric_cast<streamsize>(get_width()) *
      boost::numeric_cast<streamsize>(get_height()) *
      COLOR_TYPE;
    
    streamsize written = 0;
    
    streamsize write_size = WRITE_SIZE;
    
    uint8_t *nil = new uint8_t[write_size];
    memset(nil, 0x0, WRITE_SIZE);
    
    reporter.set_limit(total);
    
    while (written < total) {
      streamsize write = min(total, write_size);
      fs.write(reinterpret_cast<char*>(nil), write);
      reporter.add(write);
      written += write;
    }
    
    reporter.done(0);
    
    delete [] nil;
  }
  
  ~cached_image() {
    flush_buffer();
    fs.close();
  }
  
  void set_pixel(pos_t x, pos_t y, color&);
  void get_pixel(pos_t x, pos_t y, color&);

  /*
   * This is where you may use caching or whatever mechanism.
   *
   * It is guaranteed that no other points will be used
   * except for those that has previously been declared
   * with 'align'.
   *
   * @see #align
   **/
  void blend_pixel(pos_t x, pos_t y, color &c);

  /*
   * Read a list of colors.
   * The pointer color must contain all necessary colors.
   * 
   * The rationale for this method is that it usually is much more efficient
   * to have a continious set of colors read/written since it usually is more
   * efficient. While letting the single operations be made against memory.
   *
   * This method must assert that the result reflects the same as the backend
   * store, usually means that you actually have to read from it.
   **/
  void get_line(pos_t y, pos_t x, pos_t width, color*);
  
  /*
   * Set a list of colors.
   * @see #get_line
   **/
  void set_line(pos_t y, pos_t x, pos_t width, color*);
  
  /*
   * Align whatever caching mechanism might be used to only expect blend requests for these areas.
   **/
  void align(pos_t x, pos_t y, pos_t w, pos_t h);
};

#endif /* CACHED_IMAGE */
