#ifndef FATISO_ENGINE
#define FATISO_ENGINE

#include "engine/isometric_base.hpp"

class fatiso_engine : public isometric_base<fatiso_cube> {
public:
  fatiso_engine(engine_settings& s, mc::world& world);

  void project_limits(
      pos_t& image_width,
      pos_t& image_height
      );

  void project_position(
      point& p,
      pos_t& image_x,
      pos_t& image_y
      );

  void project_world_limits(
      pos_t& image_width,
      pos_t& image_height
      );

  void project_world_position(
      point& p,
      pos_t& image_x,
      pos_t& image_y
      );

  void render_block(
      image_operations_ptr o,
      int bt,
      pos_t px,
      pos_t py,
      color top,
      color side
      );

  void render_halfblock(
      image_operations_ptr o,
      int bt,
      pos_t px,
      pos_t py,
      color top,
      color side
      );

  void render_torchblock(
      image_operations_ptr o,
      int bt,
      pos_t px,
      pos_t py,
      color top,
      color side
      );
};

#endif /* FATISO_ENGINE */
