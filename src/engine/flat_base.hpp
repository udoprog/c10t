#ifndef _ENGINE_FLAT_BASE_HPP
#define _ENGINE_FLAT_BASE_HPP

#include "engine/engine_base.hpp"
#include "engine/block_rotation.hpp"
#include "engine/functions.hpp"

template<typename C>
class flat_base : public engine_base<C> {
public:
  typedef uint64_t pos_t;
  typedef boost::shared_ptr<mc::level> level_ptr;
  typedef boost::shared_ptr<image_operations> image_operations_ptr;

  flat_base(engine_settings& s, mc::world& world)
    : engine_base<C>(s, world)
  {
  }

  void render(level_ptr level, image_operations_ptr oper)
  {
    const engine_settings& s = flat_base<C>::get_settings();

    pos_t iw, ih;
    flat_base<C>::get_level_boundaries(iw, ih);

    // block type
    block_rotation b_r(s.rotation, level->get_blocks());
    block_rotation b_d(s.rotation, level->get_data());
    block_rotation bl_r(s.rotation, level->get_blocklight());
    block_rotation sl_r(s.rotation, level->get_skylight());
    
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
          for (int y = s.top; y >= s.bottom && hell_solid; y--) {
            hell_solid = !is_open(b_r.get8(y));
          }
        }
        
        // do incremental color fill until color is opaque
        for (int y = s.top; y >= s.bottom; y--) {
          int bt = b_r.get8(y);
          
          if (s.cavemode && cave_ignore_block(y, bt, b_r, cave_initial)) {
            continue;
          }
          
          if (s.hellmode && !hell_solid && hell_ignore_block(y, bt, b_r, hell_initial))
          {
            continue;
          }
          
          if (s.excludes[bt]) {
            continue;
          }
          
          color top = blockColor_top(bt, y, b_d);
          color side = blockColor_side(bt, y, b_d);

          apply_shading(s, bl_r.get4(y + 1), sl_r.get4(y + 1), 0, y, top);
          
          point p(x, y, z);
          
          pos_t px;
          pos_t py;

          flat_base<C>::project_position(p, px, py);
          
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
          
          if (top.is_opaque()) {
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

#endif /* _ENGINE_FLAT_BASE_HPP */
