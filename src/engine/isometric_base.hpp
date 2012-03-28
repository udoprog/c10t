#ifndef _ENGINE_ISOMETRIC_BASE_HPP
#define _ENGINE_ISOMETRIC_BASE_HPP

#include "engine/engine_base.hpp"
#include "engine/block_rotation.hpp"
#include "engine/functions.hpp"

template<typename C>
class isometric_base : public engine_base<C> {
public:
  typedef uint64_t pos_t;
  typedef boost::shared_ptr<mc::level> level_ptr;
  typedef boost::shared_ptr<image_operations> image_operations_ptr;

  isometric_base(engine_settings& s, mc::world& world)
    : engine_base<C>(s, world)
  {
  }

  void render(level_ptr level, image_operations_ptr oper)
  {
    const engine_settings& s = engine_base<C>::get_settings();

    block_rotation<nbt::ByteArray, int> b_r(s.rotation, level->get_blocks());
    block_rotation<nbt::ByteArray, int> b_d(s.rotation, level->get_data());
    block_rotation<nbt::ByteArray, int> bl_r(s.rotation, level->get_blocklight());
    block_rotation<nbt::ByteArray, int> sl_r(s.rotation, level->get_skylight());
    block_rotation<nbt::IntArray, int> hm_r(s.rotation, level->get_heightmap());

    pos_t image_width = 0;
    pos_t image_height = 0;
    
    engine_base<C>::project_limits(image_width, image_height);

    pos_t bmt = image_width * image_height;
    
    boost::scoped_array<bool> blocked(new bool[bmt]);
    memset(blocked.get(), 0x0, sizeof(bool) * bmt);
    
    oper->set_limits(image_width + 1, image_height);

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
          for (int y = s.top; y >= s.bottom && hell_solid; y--) {
            hell_solid = !is_open(b_r.get8(y));
          }
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
          
          pos_t px = 0;
          pos_t py = 0;
          engine_base<C>::project_position(p, px, py);
          
          color top = blockColor_top(bt, y, b_d),
               side = blockColor_side(bt, y, b_d);
          
          if (mc::MaterialModes[bt] == mc::Block) {
            int bp = px + image_width * py;
            
            if (blocked[bp]) {
              continue;
            }
            
            blocked[bp] = top.is_opaque() && bt != mc::Fence;
          }
          
          
          int bl = bl_r.get4(y + 1);
          
          apply_shading(s, bl, sl_r.get4(y + 1), hmval, y, top);
          apply_shading(s, bl, -1, hmval, y, side);
          
          switch(mc::MaterialModes[bt]) {
          case mc::Block:
            render_block(oper, bt, px, py, top, side);
          case mc::HalfBlock:
            render_halfblock(oper, bt, px, py, top, side);
            break;
          case mc::TorchBlock:
            render_torchblock(oper, bt, px, py, top, side);
            break;
          }
        }
      }
    }
  }

  virtual void render_block(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
  virtual void render_halfblock(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
  virtual void render_torchblock(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
};

#endif /* _ENGINE_ISOMETRIC_BASE_HPP */
