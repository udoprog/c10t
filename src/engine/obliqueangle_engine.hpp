#ifndef OBLIQUEANGLE_ENGINE
#define OBLIQUEANGLE_ENGINE

#include "engine/isometric_base.hpp"

typedef angle_cube<1,1,1> obliqueangle_cube;
class obliqueangle_engine : public isometric_base<obliqueangle_cube> {
public:
  obliqueangle_engine(engine_settings& s, mc::world& world);

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

#endif /* OBLIQUE_ENGINE */
