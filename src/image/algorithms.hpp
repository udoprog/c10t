// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef __IMAGE_ALGORITHMS__
#define __IMAGE_ALGORITHMS__

#include "image/image_base.hpp"
#include "image/virtual_image.hpp"

#include "2d/cube.hpp"

#include <map>

namespace image {
  /**
   * Split an image into multiple smaller images based on a specific pixel size.
   */
  template<typename M>
  void split(image_ptr base, int pixels, M& map)
  {
    for (image_base::pos_t w = 0, px = 0; w < base->get_width(); w += pixels, px++) {
      for (image_base::pos_t h = 0, py = 0; h < base->get_height(); h += pixels, py++) {
        map[point2(px, py)] = new virtual_image(pixels, pixels, base, w, h);
      }
    }
  }
}

#endif /*__IMAGE_ALGORITHMS__*/
