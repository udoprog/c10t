#include "image/algorithms.hpp"

namespace image {
  image_ptr crop(image_ptr base, pos_t min_x, pos_t max_x, pos_t min_y, pos_t max_y)
  {
    image_ptr ptr(new virtual_image(max_x - min_x, max_y - min_y, base, min_x, min_y));
    return ptr;
  }
}
