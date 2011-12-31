#ifndef ISOMETRIC_ENGINE
#define ISOMETRIC_ENGINE

#include "engine/isometric_base.hpp"

class isometric_engine : public isometric_base<isometric_cube> {
public:
  isometric_engine(engine_settings& s, mc::world& world);

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

#endif /* ISOMETRIC_ENGINE */
