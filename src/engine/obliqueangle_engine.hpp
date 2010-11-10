#ifndef OBLIQUEANGLE_ENGINE
#define OBLIQUEANGLE_ENGINE

#include "engine/engine_base.hpp"

class obliqueangle_engine : public engine_base {
  public:
    obliqueangle_engine(settings_t& s) : engine_base(s) {}
    void render(level_file& level, boost::shared_ptr<image_operations> operations);
};

#endif /* OBLIQUE_ENGINE */
