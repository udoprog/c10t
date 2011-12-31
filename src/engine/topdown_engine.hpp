#ifndef TOPDOWN_ENGINE
#define TOPDOWN_ENGINE

#include "engine/engine_base.hpp"
#include "engine/engine_core.hpp"

class topdown_engine : public engine_base<top_cube> {
  public:
    topdown_engine(engine_settings& s, mc::world& world) : engine_base<top_cube>(s, world) {}
    void render(level_ptr level, boost::shared_ptr<image_operations> operations);
};

#endif /* TOPDOWN_ENGINE */
