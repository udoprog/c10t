#include "engine/topdown_engine.hpp"
#include <iostream>

void topdown_engine::render(level_ptr level, boost::shared_ptr<image_operations> oper)
{
  pos_t iw, ih;
  get_level_boundaries(iw, ih);
  
  // block type
  BlockRotation b_r(s, level->get_blocks());
  BlockRotation b_d(s, level->get_data());
  BlockRotation bl_r(s, level->get_blocklight());
  BlockRotation sl_r(s, level->get_skylight());
  
  oper->set_limits(iw, ih);
  
  for (int z = 0; z < mc::MapZ; z++) {
    for (int x = 0; x < mc::MapX; x++) {
      bool cave_initial = true;
      bool hell_initial = true;
      bool hell_solid = true;
      
      b_r.set_xz(x, z);
      b_d.set_xz(x, z);
      bl_r.set_xz(x, z);
      sl_r.set_xz(x, z);
      
      if (s.hellmode) {
        for (int y = s.top; y >= s.bottom && hell_solid; y--) { hell_solid = !is_open(b_r.get8(y)); }
      }
      
      // do incremental color fill until color is opaque
      for (int y = s.top; y >= s.bottom; y--) {
        int bt = b_r.get8(y);
        
        if (s.cavemode && cave_ignore_block(s, y, bt, b_r, cave_initial)) {
          continue;
        }
        
        if (s.hellmode && !hell_solid && hell_ignore_block(s, y, bt, b_r, hell_initial)) {
          continue;
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        color bc;
        if (bt == mc::Wool) {
          int md = b_d.get4(y);
          bc = mc::WoolColor[md];
        } else if ((bt == mc::Step) || (bt == mc::DoubleStep)) {
          int md = b_d.get4(y);
          bc = mc::StepColor[md];
        } else {
          bc = mc::MaterialColor[bt];
        }

        apply_shading(s, bl_r.get4(y + 1), sl_r.get4(y + 1), 0, y, bc);
        
        point p(x, y, z);
        
        pos_t px;
        pos_t py;

        part_c.project_top(p, px, py);
        
        oper->add_pixel(px, py, bc);
        
        if (bc.is_opaque()) {
          break;
        }
      }
    }
  }
  
  return;
}
