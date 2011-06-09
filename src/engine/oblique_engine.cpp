#include "engine/oblique_engine.hpp"

#include <boost/scoped_array.hpp>

void oblique_engine::render(level_ptr level, boost::shared_ptr<image_operations> oper)
{
  Cube part_c(mc::MapX + 1, mc::MapY + 1, mc::MapZ + 1);
  
  pos_t iw, ih;
  part_c.get_oblique_limits(iw, ih);
  
  BlockRotation b_r(s, level->get_blocks());
  BlockRotation b_d(s, level->get_data());
  BlockRotation bl_r(s, level->get_blocklight());
  BlockRotation sl_r(s, level->get_skylight());
  
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
        
        if (s.cavemode && cave_ignore_block(s, y, bt, b_r, cave_initial)) {
          continue;
        }
        
        if (s.hellmode && !hell_solid && hell_ignore_block(s, y, bt, b_r, hell_initial)) {
          continue;
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        point p(x, y, z);
        
        pos_t px, py;
        part_c.project_oblique(p, px, py);
        
        color top, side;
        if (bt == mc::Wool) {
          int md = b_d.get4(y);
          top = mc::WoolColor[md];
          side = mc::WoolColor[md];
        } else if ((bt == mc::Step) || (bt == mc::DoubleStep)) {
          int md = b_d.get4(y);
          top = mc::StepColor[md];
          side = mc::StepColor[md];
        } else {
          top = mc::MaterialColor[bt];
          side = mc::MaterialSideColor[bt];
        }
        
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
