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
public:
  typedef uint64_t pos_t;
  typedef boost::shared_ptr<image_base> image_ptr;
protected:
  pos_t w, h;
public:
  typedef void (*progress_c)(int , int);
  
  static const short COLOR_TYPE = 4;
  
  image_base(pos_t w, pos_t h) : w(w), h(h) {
  }
  
  virtual ~image_base() {
  }
  
  void fill(color& c);

  void clear();
  
  inline pos_t get_width() { return w; };
  inline pos_t get_height() { return h; };
  
  void composite(int xoffset, int yoffset, boost::shared_ptr<image_operations> oper);
  
  inline std::streampos get_offset(std::streampos x, std::streampos y) {
    std::streampos width = get_width();
    return (x * sizeof(color)) + (y * width * sizeof(color));
  }
  
  bool save_png(const std::string filename, const char *title, progress_c);
  
  void safe_blend_pixel(pos_t x, pos_t y, color &c);

  void get_line(pos_t y, color *c) {
    get_line(y, 0, get_width(), c);
  }

  template<typename T>
  bool save(const std::string str, typename T::opt_type opts) {
    return T::save(this, str, opts);
  }

  void resize(image_ptr target);
  
  virtual void blend_pixel(pos_t x, pos_t y, color &c) = 0;
  virtual void set_pixel(pos_t x, pos_t y, color& c) = 0;
  virtual void get_pixel(pos_t x, pos_t y, color& c) = 0;
  virtual void get_line(pos_t y, pos_t offset, pos_t w, color*) = 0;
  virtual void set_line(pos_t y, pos_t offset, pos_t w, color*) {};
  virtual void align(pos_t x, pos_t y, pos_t w, pos_t h) {};
};

std::map<point2, image_base*> image_split(image_base* base, int pixels);

#endif /* IMAGE_BASE */
