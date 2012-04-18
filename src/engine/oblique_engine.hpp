#ifndef OBLIQUE_ENGINE
#define OBLIQUE_ENGINE

#include "engine/engine_base.hpp"

class oblique_engine : public engine_base<oblique_cube> {
  public:
    oblique_engine(engine_settings& s, mc::world& world) : engine_base<oblique_cube>(s, world) {}
    void render(level_ptr level, boost::shared_ptr<image_operations> operations, mc::utils::level_coord nonrotated_coord);
};

#endif /* OBLIQUE_ENGINE */
