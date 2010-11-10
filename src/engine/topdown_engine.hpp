#ifndef TOPDOWN_ENGINE
#define TOPDOWN_ENGINE

#include "engine/engine_base.hpp"

class topdown_engine : public engine_base {
  public:
    topdown_engine(settings_t& s) : engine_base(s) {}
    void render(level_file& level, boost::shared_ptr<image_operations> operations);
};

#endif /* TOPDOWN_ENGINE */
