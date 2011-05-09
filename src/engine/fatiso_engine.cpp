#include "engine/fatiso_engine.hpp"

#include "boost/scoped_array.hpp"

void fatiso_engine::render(level_ptr level, boost::shared_ptr<image_operations> oper)
{
  BlockRotation b_r(s, level->get_blocks());
  BlockRotation b_d(s, level->get_data());
  BlockRotation bl_r(s, level->get_blocklight());
  BlockRotation sl_r(s, level->get_skylight());
  BlockRotation hm_r(s, level->get_heightmap());

  pos_t iw, ih;
  
  part_c.get_fatiso_limits(iw, ih);
  pos_t bmt = iw * ih;
  
  boost::scoped_array<bool> blocked(new bool[bmt]);
  memset(blocked.get(), 0x0, sizeof(bool) * bmt);
  
  oper->set_limits(iw + 1, ih);
  
  for (int z = mc::MapZ - 1; z >= 0; z--) {
    for (int x = mc::MapX - 1; x >= 0; x--) {
      bool cave_initial = true;
      bool hell_initial = true;
      bool hell_solid = true;
      
      hm_r.set_xz(x, z);
      b_r.set_xz(x, z);
      b_d.set_xz(x, z);
      bl_r.set_xz(x, z);
      sl_r.set_xz(x, z);
      
      int hmval = hm_r.get8();
      
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
        part_c.project_fatiso(p, px, py);
        
        color top, side;
        if(bt == mc::Wool) {
          int md = b_d.get4(y);
          top = mc::WoolColor[md];
          side = mc::WoolColor[md];
        } else {
          top = mc::MaterialColor[bt];
          side = mc::MaterialSideColor[bt];
        }
        
        if (mc::MaterialModes[bt] == mc::Block) {
          int bp = px + iw * py;
          
          if (blocked[bp]) {
            continue;
          }
          
          blocked[bp] = top.is_opaque() && bt != mc::Fence;
        }
        
        
        int bl = bl_r.get4(y + 1);
        
        apply_shading(s, bl, sl_r.get4(y + 1), hmval, y, top);
        apply_shading(s, bl, -1, hmval, y, side);

        color topdark(top);
        color toplight(top);
        color sidelight(side);

        if (bt == mc::Grass) {
          topdark.darken(0x20);
          toplight.darken(0x10);
          sidelight.lighten(0x20);
        }
        else {
          toplight = color(side);
          topdark = color(side);
        }

        oper->add_pixel(px + 0, py + 0, side);
        oper->add_pixel(px + 0, py + 1, side);
        oper->add_pixel(px + 0, py + 2, side);
        oper->add_pixel(px + 1, py - 1, side);
        oper->add_pixel(px + 1, py + 0, side);
        oper->add_pixel(px + 1, py + 1, side);
        oper->add_pixel(px + 2, py - 1, side);
        oper->add_pixel(px + 2, py + 0, side);
        oper->add_pixel(px + 2, py + 1, side);
        oper->add_pixel(px + 3, py - 2, side);
        oper->add_pixel(px + 3, py - 1, side);
        oper->add_pixel(px + 3, py + 0, side);

        oper->add_pixel(px - 1, py + 0, sidelight);
        oper->add_pixel(px - 1, py + 1, sidelight);
        oper->add_pixel(px - 1, py + 2, sidelight);
        oper->add_pixel(px - 2, py - 1, sidelight);
        oper->add_pixel(px - 2, py + 0, sidelight);
        oper->add_pixel(px - 2, py + 1, sidelight);
        oper->add_pixel(px - 3, py - 1, sidelight);
        oper->add_pixel(px - 3, py + 0, sidelight);
        oper->add_pixel(px - 3, py + 1, sidelight);
        oper->add_pixel(px - 4, py - 2, sidelight);
        oper->add_pixel(px - 4, py - 1, sidelight);
        oper->add_pixel(px - 4, py + 0, sidelight);

        oper->add_pixel(px + 0, py - 2, topdark);
        oper->add_pixel(px + 0, py - 1, topdark);
        oper->add_pixel(px + 1, py - 3, topdark);
        oper->add_pixel(px + 1, py - 2, topdark);
        oper->add_pixel(px + 2, py - 3, topdark);
        oper->add_pixel(px + 2, py - 2, topdark);
        oper->add_pixel(px + 3, py - 4, topdark);
        oper->add_pixel(px + 3, py - 3, topdark);

        oper->add_pixel(px - 1, py - 2, toplight);
        oper->add_pixel(px - 1, py - 1, toplight);
        oper->add_pixel(px - 2, py - 3, toplight);
        oper->add_pixel(px - 2, py - 2, toplight);
        oper->add_pixel(px - 3, py - 3, toplight);
        oper->add_pixel(px - 3, py - 2, toplight);
        oper->add_pixel(px - 4, py - 4, toplight);
        oper->add_pixel(px - 4, py - 3, toplight);

        oper->add_pixel(px - 3, py - 5, top);
        oper->add_pixel(px - 3, py - 4, top);
        oper->add_pixel(px - 2, py - 5, top);
        oper->add_pixel(px - 2, py - 4, top);
        oper->add_pixel(px - 1, py - 6, top);
        oper->add_pixel(px - 1, py - 5, top);
        oper->add_pixel(px - 1, py - 4, top);
        oper->add_pixel(px - 1, py - 3, top);
        oper->add_pixel(px + 0, py - 6, top);
        oper->add_pixel(px + 0, py - 5, top);
        oper->add_pixel(px + 0, py - 4, top);
        oper->add_pixel(px + 0, py - 3, top);
        oper->add_pixel(px + 1, py - 5, top);
        oper->add_pixel(px + 1, py - 4, top);
        oper->add_pixel(px + 2, py - 5, top);
        oper->add_pixel(px + 2, py - 4, top);
      }
    }
  }
  
  return;
}
