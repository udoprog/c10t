#include "engine/topdown_engine.hpp"

void topdown_engine::render(level_file& level, boost::shared_ptr<image_operations> oper)
{
  if (!level.islevel) {
    return;
  }
  
  size_t iw, ih;
  part_c.get_top_limits(iw, ih);
  
  // block type
  BlockRotation b_r(s, level.blocks.get());
  BlockRotation bl_r(s, level.blocklight.get());
  BlockRotation sl_r(s, level.skylight.get());
  
  oper->set_limits(iw, ih);
  
  for (int z = 0; z < mc::MapZ; z++) {
    for (int x = 0; x < mc::MapX; x++) {
      bool cave_initial = true;

      b_r.set_xz(x, z);
      bl_r.set_xz(x, z);
      sl_r.set_xz(x, z);
      
      // do incremental color fill until color is opaque
      for (int y = s.top; y > s.bottom; y--) {
        int bt = b_r.get8(y);
        
        if (s.cavemode && cave_ignore_block(s, y, bt, b_r, cave_initial)) {
          continue;
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        color bc = mc::MaterialColor[bt];
        
        apply_shading(s, bl_r.get4(y + 1), sl_r.get4(y + 1), 0, y, bc);
        
        point p(x, y, z);
        
        size_t px;
        size_t py;

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
