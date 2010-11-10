#ifndef MEMORY_IMAGE
#define MEMORY_IMAGE

#include "image/image_base.hpp"
#include "image/color.hpp"

class memory_image : public image_base {
private:
  uint8_t *colors;
public:
  memory_image(int w, int h);
  
  ~memory_image();
  
  void blend_pixel(size_t x, size_t y, color &c);
  void set_pixel(size_t x, size_t y, color&);
  void get_pixel(size_t x, size_t y, color&);
  void get_line(size_t y, size_t offset, size_t width, color*);
};

#endif /* MEMORY_IMAGE */
