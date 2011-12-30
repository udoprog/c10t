#include "engine/oblique_engine.hpp"
#include "engine/block_rotation.hpp"
#include "engine/functions.hpp"

#include <boost/scoped_array.hpp>

void oblique_engine::render(level_ptr level, boost::shared_ptr<image_operations> oper)
{
  Cube part_c(mc::MapX + 1, mc::MapY + 1, mc::MapZ + 1);
  
  pos_t iw, ih;
  part_c.get_oblique_limits(iw, ih);
  
  const engine_settings& s = get_settings();
  
  block_rotation b_r(s.rotation, level->get_blocks());
  block_rotation b_d(s.rotation, level->get_data());
  block_rotation bl_r(s.rotation, level->get_blocklight());
  block_rotation sl_r(s.rotation, level->get_skylight());
  
  pos_t bmt = iw * ih;
  
  boost::scoped_array<bool> blocked(new bool[bmt]);
  memset(blocked.get(), 0x0, sizeof(bool) * bmt);
  
  oper->set_limits(iw + 1, ih);
  
  for (int z = mc::MapZ - 1; z >= 0; z--) {
    for (int x = mc::MapX - 1; x >= 0; x--) {
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
      
      for (int y = s.top; y >= s.bottom; y--) {
        int bt = b_r.get8(y);
        
        if (s.cavemode && cave_ignore_block(y, bt, b_r, cave_initial)) {
          continue;
        }
        
        if (s.hellmode && !hell_solid && hell_ignore_block(y, bt, b_r, hell_initial)) {
          continue;
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        point p(x, y, z);
        
        pos_t px, py;
        part_c.project_oblique(p, px, py);
        
        color top = blockColor_top(bt, y, b_d),
             side = blockColor_side(bt, y, b_d);
        
        int bp = px + iw * py;
        
        if (blocked[bp]) {
          continue;
        }
        
        blocked[bp] = top.is_opaque();
        
        int bl = bl_r.get4(y + 1);
        
        apply_shading(s, bl, sl_r.get4(y + 1), 0, y, top);
        oper->add_pixel(px, py, top);
        
        apply_shading(s, bl, -1, 0, y, side);
        oper->add_pixel(px, py + 1, side);
      }
    }
  }
}
