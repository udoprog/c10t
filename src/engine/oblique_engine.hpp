#ifndef OBLIQUE_ENGINE
#define OBLIQUE_ENGINE

#include "engine/engine_base.hpp"

class oblique_engine : public engine_base {
  public:
    oblique_engine(settings_t& s) : engine_base(s) {}
    void render(level_file& level, boost::shared_ptr<image_operations> operations);
};

#endif /* OBLIQUE_ENGINE */
