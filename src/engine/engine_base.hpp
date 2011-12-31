#ifndef _ENGINE_ENGINE_BASE_HPP
#define _ENGINE_ENGINE_BASE_HPP

#include "engine/engine_settings.hpp"
#include "engine/engine_core.hpp"

#include "2d/cube.hpp"
#include "mc/level.hpp"
#include "mc/world.hpp"
#include "mc/blocks.hpp"
#include "image/image_operations.hpp"

template<typename C>
class engine_base : public engine_core {
  public:
    engine_base(engine_settings engine_s, mc::world& world) :
      part_c(mc::MapX + 1, mc::MapY + 1, mc::MapZ + 1),
      pos_c(world.diff_x * mc::MapX, mc::MapY, world.diff_z * mc::MapZ),
      mpos_c((world.diff_x + 1) * mc::MapX, mc::MapY, (world.diff_z + 1) * mc::MapZ),
      engine_s(engine_s),
      world(world)
    {
    }

    void project_limits(pos_t& image_width, pos_t& image_height) {
      part_c.limits(image_width, image_height);
    }

    void project_position(point& p, pos_t& image_x, pos_t& image_y) {
      part_c.project(p, image_x, image_y);
    }

    void get_boundaries(pos_t& width, pos_t& height) {
      mpos_c.limits(width, height);
    }

    void get_level_boundaries(pos_t& width, pos_t& height) {
      part_c.limits(width, height);
    }

    void w2pt(int xPos, int zPos, pos_t& x, pos_t& y) {
      pos_t posx = xPos - get_world().min_x;
      pos_t posz = zPos - get_world().min_z;

      point pos(posx * mc::MapX, mc::MapY, posz * mc::MapZ);

      pos_c.project(pos, x, y);
    }
    
    void wp2pt(int xPos, int yPos, int zPos, pos_t& x, pos_t& y) {
      point pos(xPos - get_world().min_xp, yPos, zPos - get_world().min_zp);
      mpos_c.project(pos, x, y);
      x -= im_min_x;
      y -= im_min_y;
    }

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

    pos_t get_min_x() {
      return im_min_x;
    }

    pos_t get_max_x() {
      return im_max_x;
    }

    pos_t get_min_y() {
      return im_min_y;
    }

    pos_t get_max_y() {
      return im_max_y;
    }
  private:
    const C part_c;
    const C pos_c;
    const C mpos_c;

    const engine_settings engine_s;
    const mc::world& world;

    pos_t im_min_x;
    pos_t im_min_y;
    pos_t im_max_x;
    pos_t im_max_y;
};

#endif /* _ENGINE_ENGINE_BASE_HPP */
