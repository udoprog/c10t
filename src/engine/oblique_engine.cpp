#include "engine/oblique_engine.hpp"

void oblique_engine::render(level_file& level, boost::shared_ptr<image_operations> oper)
{
  if (!level.islevel) {
    return;
  }
  
  Cube c(mc::MapX + 1, mc::MapY + 1, mc::MapZ + 1);
  
  size_t iw, ih;
  c.get_oblique_limits(iw, ih);
  
  BlockRotation b_r(s, level.blocks.get());
  BlockRotation bl_r(s, level.blocklight.get());
  BlockRotation sl_r(s, level.skylight.get());
  
  size_t bmt = iw * ih;
  
  bool* blocked = new bool[bmt];
  memset(blocked, 0x0, sizeof(bool) * bmt);
  
  oper->set_limits(iw + 1, ih);
  
  for (int z = mc::MapZ - 1; z >= 0; z--) {
    for (int x = mc::MapX - 1; x >= 0; x--) {
      bool cave_initial = true;
      
      b_r.set_xz(x, z);
      bl_r.set_xz(x, z);
      sl_r.set_xz(x, z);
      
      for (int y = s.top; y >= s.bottom; y--) {
        int bt = b_r.get8(y);
        
        if (s.excludes[bt]) {
          continue;
        }
        
        if (s.cavemode && cave_ignore_block(s, y, bt, b_r, cave_initial)) {
          continue;
        }

        point p(x, y, z);
        
        size_t px, py;
        c.project_oblique(p, px, py);
        
        color top = mc::MaterialColor[bt];
        
        int bp = px + iw * py;
        
        if (blocked[bp]) {
          continue;
        }
        
        blocked[bp] = top.is_opaque();
        
        int bl = bl_r.get4(y + 1);
        
        apply_shading(s, bl, sl_r.get4(y + 1), 0, y, top);
        oper->add_pixel(px, py, top);
        
        color side = mc::MaterialSideColor[bt];
        apply_shading(s, bl, -1, 0, y, side);
        oper->add_pixel(px, py + 1, side);
      }
    }
  }
}
