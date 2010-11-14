#include "engine/isometric_engine.hpp"

#include "boost/scoped_array.hpp"

void isometric_engine::render(level_file& level, boost::shared_ptr<image_operations> oper)
{
  if (!level.islevel) {
    return;
  }
  
  BlockRotation b_r(s, level.blocks.get());
  BlockRotation bl_r(s, level.blocklight.get());
  BlockRotation sl_r(s, level.skylight.get());
  BlockRotation hm_r(s, level.heightmap.get());

  pos_t iw, ih;
  
  part_c.get_isometric_limits(iw, ih);
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
        part_c.project_isometric(p, px, py);
        
        color top = mc::MaterialColor[bt];
        
        if (mc::MaterialModes[bt] == mc::Block) {
          int bp = px + iw * py;
          
          if (blocked[bp]) {
            continue;
          }
          
          blocked[bp] = top.is_opaque() && bt != mc::Fence;
        }
        
        color side = mc::MaterialSideColor[bt];
        
        int bl = bl_r.get4(y + 1);
        
        apply_shading(s, bl, sl_r.get4(y + 1), hmval, y, top);
        apply_shading(s, bl, -1, hmval, y, side);
        
        switch(mc::MaterialModes[bt]) {
        case mc::Block:
          oper->add_pixel(px, py, top);
          oper->add_pixel(px + 1, py, top);
          oper->add_pixel(px - 2, py, top);
          oper->add_pixel(px - 1, py, top);
          
          oper->add_pixel(px - 2, py + 1, side);
          
          if (bt != mc::Fence) {
            oper->add_pixel(px - 1, py + 1, side);
            oper->add_pixel(px - 1, py + 2, side);
          }
          
          oper->add_pixel(px - 2, py + 2, side);
          
          side.lighten(0x20);
          
          if (bt != mc::Fence) {
            oper->add_pixel(px, py + 1, side);
            oper->add_pixel(px, py + 2, side);
          }
          
          oper->add_pixel(px + 1, py + 1, side);
          oper->add_pixel(px + 1, py + 2, side);
          break;
        case mc::HalfBlock:
          oper->add_pixel(px, py + 1, top);
          oper->add_pixel(px + 1, py + 1, top);
          oper->add_pixel(px - 2, py + 1, top);
          oper->add_pixel(px - 1, py + 1, top);
          
          oper->add_pixel(px - 2, py + 2, side);
          oper->add_pixel(px - 1, py + 2, side);
          
          side.lighten(0x20);
          
          oper->add_pixel(px, py + 2, side);
          oper->add_pixel(px + 1, py + 2, side);
          break;
        case mc::TorchBlock:
          oper->add_pixel(px, py, top);
          oper->add_pixel(px - 1, py, top);
          
          top.lighten(0x20);
          top.a -= 0xb0;
          
          oper->add_pixel(px, py + 1, top);
          oper->add_pixel(px - 1, py + 1, top);
          
          oper->add_pixel(px - 1, py + 1, side);
          oper->add_pixel(px - 1, py + 2, side);
          
          side.lighten(0x20);
          
          oper->add_pixel(px, py + 1, side);
          oper->add_pixel(px, py + 2, side);
          
          oper->add_pixel(px - 2, py, top);
          oper->add_pixel(px + 1, py, top);
          oper->add_pixel(px, py - 1, top);
          oper->add_pixel(px - 1, py - 1, top);
          break;
        }
      }
    }
  }
  
  return;
}
