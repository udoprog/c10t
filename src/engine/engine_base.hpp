#ifndef _ENGINE_ENGINE_BASE_HPP
#define _ENGINE_ENGINE_BASE_HPP

#include <boost/shared_ptr.hpp>

#include "engine/engine_settings.hpp"

#include "2d/cube.hpp"
#include "mc/level.hpp"
#include "mc/world.hpp"
#include "mc/blocks.hpp"
#include "image/image_operations.hpp"

class engine_base;

typedef boost::shared_ptr<engine_base> engine_ptr;

class engine_base {
  public:
    typedef uint64_t pos_t;
    typedef boost::shared_ptr<mc::level> level_ptr;
    pos_t im_min_x;
    pos_t im_min_y;
    pos_t im_max_x;
    pos_t im_max_y;
    
    engine_base(engine_settings engine_s, mc::world& world) :
      part_c(mc::MapX + 1, mc::MapY + 1, mc::MapZ + 1),
      pos_c(world.diff_x * mc::MapX, mc::MapY, world.diff_z * mc::MapZ),
      mpos_c((world.diff_x + 1) * mc::MapX, mc::MapY, (world.diff_z + 1) * mc::MapZ),
      engine_s(engine_s),
      world(world)
    {
    }
    
    virtual void render(level_ptr level, boost::shared_ptr<image_operations> oper) = 0;
    virtual void get_boundaries(pos_t& width, pos_t& height) = 0;
    virtual void get_level_boundaries(pos_t& width, pos_t& height) = 0;
    virtual void w2pt(int xPos, int zPos, pos_t& x, pos_t& y) = 0;
    virtual void wp2pt(int xPos, int yPos, int zPos, pos_t& x, pos_t& y) {
      x -= im_min_x;
      y -= im_min_y;
    };

    void reset_image_limits() {
      im_min_x = std::numeric_limits<pos_t>::max();
      im_min_y = std::numeric_limits<pos_t>::max();
      im_max_x = std::numeric_limits<pos_t>::min();
      im_max_y = std::numeric_limits<pos_t>::min();
    }

    void update_image_limits(pos_t x, pos_t y, pos_t max_x, pos_t max_y) {
      im_min_x = std::min(im_min_x, x);
      im_min_y = std::min(im_min_y, y);
      im_max_x = std::max(im_max_x, max_x);
      im_max_y = std::max(im_max_y, max_y); 
    };

    const engine_settings& get_settings() {
      return engine_s;
    }

    const mc::world& get_world() {
      return world;
    }
  protected:
    const Cube part_c, pos_c, mpos_c;
  private:
    const engine_settings engine_s;
    const mc::world& world;
};

#endif /* _ENGINE_ENGINE_BASE_HPP */
