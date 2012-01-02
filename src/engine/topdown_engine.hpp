#ifndef TOPDOWN_ENGINE
#define TOPDOWN_ENGINE

#include "engine/flat_base.hpp"

class topdown_engine : public flat_base<top_cube> {
public:
  topdown_engine(engine_settings& s, mc::world& world);

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

#endif /* TOPDOWN_ENGINE */
