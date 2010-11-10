#ifndef ISOMETRIC_ENGINE
#define ISOMETRIC_ENGINE

#include "engine/engine_base.hpp"

class isometric_engine : public engine_base {
  private:
    const Cube c;
    size_t iw, ih;
    size_t bmt;
  public:
    isometric_engine(settings_t& s) :
      engine_base(s),
      c(mc::MapX + 1, mc::MapY + 1, mc::MapZ + 1)
    {
      c.get_isometric_limits(iw, ih);
      bmt = iw * ih;
    }
    
    void render(level_file& level, boost::shared_ptr<image_operations> operations);
};

#endif /* ISOMETRIC_ENGINE */
