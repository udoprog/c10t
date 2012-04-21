#ifndef _ENGINE_FLAT_BASE_HPP
#define _ENGINE_FLAT_BASE_HPP

#include "engine/engine_base.hpp"
#include "engine/block_rotation.hpp"
#include "engine/functions.hpp"

#include <boost/foreach.hpp>

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

  void render(level_ptr level, image_operations_ptr oper, mc::utils::level_coord nonrotated_coord)
  {
    const engine_settings& s = flat_base<C>::get_settings();

    pos_t iw, ih;
    flat_base<C>::get_level_boundaries(iw, ih);

    oper->set_limits(iw, ih);

    boost::shared_ptr<mc::Level_Compound> L = level->get_level();

    bool* blocked = new bool[iw*ih];

    for (unsigned int i = 0; i < iw*ih; i++) {
      blocked[i] = false;
    }

    // block type
        
    BOOST_REVERSE_FOREACH(mc::Section_Compound Section, L->Sections) {
      block_rotation br_blocks(s.rotation, Section.Blocks);
      block_rotation br_data(s.rotation, Section.Data);
      block_rotation br_block_light(s.rotation, Section.BlockLight);
      block_rotation br_sky_light(s.rotation, Section.SkyLight);

      for (int y = 15; y >= 0; y--) {
        int abs_y = (Section.Y * 16) + y;

        for (int z = 0; z < mc::MapZ; z++) {
          for (int x = 0; x < mc::MapX; x++) {
            unsigned int blocked_position = x * iw + z;

            if (blocked[blocked_position]) {
              continue;
            }

            br_blocks.set_xz(x, z);
            br_data.set_xz(x, z);
            br_block_light.set_xz(x, z);
            br_sky_light.set_xz(x, z);

            // do incremental color fill until color is opaque
            int block_type = br_blocks.get8(y);
            int block_data = br_data.get4(y);

            if (block_type >=0 && s.excludes[block_type]) {
              continue;
            }

            color top =  mc::get_color(block_type, block_data);
            color side = mc::get_side_color(block_type, block_data);

            blocked[blocked_position] = top.is_opaque();

            /*int block_light = br_block_light.get4(y + 1);
            int sky_light = br_sky_light.get4(y + 1, 15);*/

            //apply_shading(s, block_light, sky_light, 0, abs_y, top);

            point p(x, abs_y, z);

            pos_t px;
            pos_t py;

            flat_base<C>::project_position(p, px, py);

            switch(mc::MaterialModes[block_type]) {
            case mc::Block:
              render_block(oper, block_type, px, py, top, side);
            case mc::HalfBlock:
              render_halfblock(oper, block_type, px, py, top, side);
              break;
            case mc::TorchBlock:
              render_torchblock(oper, block_type, px, py, top, side);
              break;
            }
          }
        }
      }
    }

    delete [] blocked;
    oper->reverse();
  }

  virtual void render_block(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
  virtual void render_halfblock(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
  virtual void render_torchblock(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
};

#endif /* _ENGINE_FLAT_BASE_HPP */
