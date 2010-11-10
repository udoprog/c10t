#ifndef CACHED_IMAGE
#define CACHED_IMAGE

#include <fstream>
#include <string.h>

#include <boost/numeric/conversion/cast.hpp>

#include "image/image_base.hpp"
#include "image/color.hpp"

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
  size_t buffer_size;
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
    
    buffer = new icache[buffer_size];
    
    for (size_t i = 0; i < buffer_size; i++) {
      icache ic = buffer[i];
    }
    
    delete [] nil;
  }
  
  ~cached_image() {
    // flush the memory cache
    for (size_t i = 0; i < buffer_size; i++) { 
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

#endif /* CACHED_IMAGE */
