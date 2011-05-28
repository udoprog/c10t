// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef CACHED_IMAGE
#define CACHED_IMAGE

#include <fstream>
#include <cstring>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/scoped_array.hpp>
#include <boost/filesystem.hpp>

#include "image/image_base.hpp"
#include "image/color.hpp"

#include "algorithm.hpp"

namespace fs = boost::filesystem;

class cached_image : public image_base {
private:
  const fs::path path;
  std::fstream fs;
  
  pos_t l_total;
  pos_t buffer_s;
  bool buffer_set;
  boost::scoped_array<color> buffer;
  
  pos_t buffer_w;
  pos_t buffer_h;
  pos_t buffer_x;
  pos_t buffer_y;
  
  void read_buffer();
  void flush_buffer();
public:
  static const pos_t WRITE_SIZE = 4096 * 8;
  
  cached_image(const fs::path path, pos_t w, pos_t h, pos_t l_w, pos_t l_h);
  ~cached_image();
  
  /**
   * Will build the cache from scratch, filling it with null, which coneniently fits
   * with black transparent colors.
   *
   * @reporter Any type which supports 'set_limit', 'add' and 'done' functions.
   */
  template<typename R>
  void build(R& reporter)
  {
    using namespace ::std;
    
    streampos total =
      boost::numeric_cast<streampos>(get_width()) *
      boost::numeric_cast<streampos>(get_height()) *
      COLOR_TYPE;
    
    streampos written = 0;
    
    streampos write_size = WRITE_SIZE;
    
    boost::scoped_array<char> nil(new char[write_size]);
    ::memset(nil.get(), 0x0, write_size);
    
    reporter.set_limit(total);
    
    while (written < total) {
      streampos write = min(total, write_size);
      fs.write(nil.get(), write);
      written += write;
      
      reporter.add(write);
    }
    
    reporter.done(0);
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
