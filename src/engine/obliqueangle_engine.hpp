#ifndef OBLIQUEANGLE_ENGINE
#define OBLIQUEANGLE_ENGINE

#include "engine/engine_base.hpp"

class obliqueangle_engine : public engine_base {
  public:
    obliqueangle_engine(settings_t& s, world_info& world) : engine_base(s, world) {}
    void render(level_file& level, boost::shared_ptr<image_operations> operations);
    
    void get_boundaries(size_t& width, size_t& height) {
      pos_c.get_obliqueangle_limits(width, height);
    }

    void w2pt(int xPos, int zPos, size_t& x, size_t& y) {
      size_t posx = xPos - world.min_x;
      size_t posz = zPos - world.min_z;
      
      point pos(posx * mc::MapX, mc::MapY, posz * mc::MapZ);
      
      pos_c.project_obliqueangle(pos, x, y);
    }
    
    void wp2pt(int xPos, int yPos, int zPos, size_t& x, size_t& y) {
      point pos(xPos - world.min_xp, yPos, zPos - world.min_zp);
      mpos_c.project_obliqueangle(pos, x, y);
    }
};

#endif /* OBLIQUE_ENGINE */
