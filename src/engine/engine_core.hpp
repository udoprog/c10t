#ifndef _ENGINE_ENGINE_CORE_HPP
#define _ENGINE_ENGINE_CORE_HPP

#include <stdint.h>
#include <boost/shared_ptr.hpp>

#include "mc/level.hpp"
#include "image/image_operations.hpp"
#include "mc/utils.hpp"

class engine_core;

typedef boost::shared_ptr<engine_core> engine_core_ptr;

class engine_core {
public:
  typedef uint64_t pos_t;
  typedef boost::shared_ptr<mc::level> level_ptr;
  typedef boost::shared_ptr<image_operations> image_operations_ptr;

  virtual void get_boundaries(pos_t& width, pos_t& height) = 0;
  virtual void get_level_boundaries(pos_t& width, pos_t& height) = 0;
  virtual void w2pt(int xPos, int zPos, pos_t& x, pos_t& y) = 0;
  virtual void wp2pt(int xPos, int yPos, int zPos, pos_t& x, pos_t& y) = 0;
  virtual void reset_image_limits() = 0;
  virtual void update_image_limits(pos_t x, pos_t y, pos_t max_x, pos_t max_y) = 0;
  virtual void render(level_ptr, image_operations_ptr, mc::utils::level_coord) = 0;

  virtual pos_t get_min_x() = 0;
  virtual pos_t get_max_x() = 0;
  virtual pos_t get_min_y() = 0;
  virtual pos_t get_max_y() = 0;
};

#endif /* _ENGINE_ENGINE_CORE_HPP */
