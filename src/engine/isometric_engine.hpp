#ifndef ISOMETRIC_ENGINE
#define ISOMETRIC_ENGINE

#include "engine/engine_base.hpp"

class isometric_engine : public engine_base {
  public:
    isometric_engine(settings_t& s) : engine_base(s) {}
    void render(level_file& level, boost::shared_ptr<image_operations> operations);
};

#endif /* ISOMETRIC_ENGINE */
