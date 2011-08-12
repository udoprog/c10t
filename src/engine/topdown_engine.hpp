#ifndef TOPDOWN_ENGINE
#define TOPDOWN_ENGINE

#include "engine/engine_base.hpp"

class topdown_engine : public engine_base {
  public:
    topdown_engine(settings_t& s, mc::world& world) : engine_base(s, world) {}
    void render(level_ptr level, boost::shared_ptr<image_operations> operations);
    
    void get_boundaries(pos_t& width, pos_t& height) {
      mpos_c.get_top_limits(width, height);
    }
    
    void get_level_boundaries(pos_t& width, pos_t& height) {
      part_c.get_top_limits(width, height);
    }
    
    void w2pt(int xPos, int zPos, pos_t& x, pos_t& y) {
      pos_t posx = xPos - world.min_x;
      pos_t posz = zPos - world.min_z;
      
      point pos(posx * mc::MapX, mc::MapY, posz * mc::MapZ);
      
      pos_c.project_top(pos, x, y);
    }
    
    void wp2pt(int xPos, int yPos, int zPos, pos_t& x, pos_t& y) {
      point pos(xPos - world.min_xp, yPos, zPos - world.min_zp);
      mpos_c.project_top(pos, x, y);
      engine_base::wp2pt(xPos, yPos, zPos, x, y);
    }
};

#endif /* TOPDOWN_ENGINE */
