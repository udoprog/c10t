// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef IMAGE_BASE
#define IMAGE_BASE

#include <cstdlib>
#include <map>

#include "image/image_operations.hpp"
#include "image/color.hpp"
#include "2d/cube.hpp"

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

namespace fs = boost::filesystem;

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
  
  void composite(int xoffset, int yoffset, boost::shared_ptr<image_operations> oper);
  
  inline std::streamsize get_offset(std::streamsize x, std::streamsize y) {
    return (x * sizeof(color)) + (y * get_width() * sizeof(color));
  }
  
  bool save_png(const std::string filename, const char *title, progress_c);
  
  void safe_blend_pixel(size_t x, size_t y, color &c);

  void get_line(size_t y, color *c) {
    get_line(y, 0, get_width(), c);
  }

  template<typename T>
  bool save(const std::string str, typename T::opt_type opts) {
    return T::save(this, str, opts);
  }
  
  virtual void blend_pixel(size_t x, size_t y, color &c) = 0;
  virtual void set_pixel(size_t x, size_t y, color& c) = 0;
  virtual void get_pixel(size_t x, size_t y, color& c) = 0;
  virtual void get_line(size_t y, size_t offset, size_t w, color*) = 0;
  virtual void set_line(size_t y, size_t offset, size_t w, color*) {};
  virtual void align(size_t x, size_t y, size_t w, size_t h) {};
};

std::map<point2, image_base*> image_split(image_base* base, int pixels);

#endif /* IMAGE_BASE */
