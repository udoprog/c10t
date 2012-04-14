// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef IMAGE_BASE
#define IMAGE_BASE

#include <cstdlib>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include "image/image_operations.hpp"
#include "image/color.hpp"

class image_base;

typedef boost::shared_ptr<image_base> image_ptr;
typedef uint64_t pos_t;

class image_base
{
protected:
  pos_t width, height;
public:
  typedef void (*progress_c)(int , int);
  
  image_base(pos_t width, pos_t height)
      : width(width), height(height) {
  }
  
  virtual ~image_base() {
  }
  
  void fill(color& c);

  void clear();
  
  inline pos_t get_width() { return width; };
  inline pos_t get_height() { return height; };
  
  void composite(int xoffset, int yoffset, image_operations_ptr oper);
  
  inline std::streampos get_offset(std::streampos x, std::streampos y) {
    std::streampos width = get_width();
    return x + y * width;
  }
  
  void safe_blend_pixel(pos_t x, pos_t y, color &c);

  void get_line(pos_t y, color *c) {
    get_line(y, 0, get_width(), c);
  }

  template<typename T>
  void save(const std::string str, typename T::opt_type opts) {
    T::save(this, str, opts);
  }

  void draw_line(pos_t x1, pos_t y1, pos_t x2, pos_t y2, color &c);

  void resize(image_ptr target);
  
  virtual void blend_pixel(pos_t x, pos_t y, color &c) = 0;
  virtual void set_pixel(pos_t x, pos_t y, color& c) = 0;
  virtual void get_pixel(pos_t x, pos_t y, color& c) = 0;
  virtual void get_line(pos_t y, pos_t offset, pos_t width, color*) = 0;
  virtual void set_line(pos_t y, pos_t offset, pos_t width, color*) {};
  virtual void align(pos_t x, pos_t y, pos_t width, pos_t height) {};
};

#endif /* IMAGE_BASE */
