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

  void render(level_ptr level, image_operations_ptr oper)
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

    BOOST_REVERSE_FOREACH(boost::shared_ptr<mc::Section_Compound> Section, L->Sections) {
      block_rotation br_blocks(s.rotation);

      for (int y = 15; y >= 0; y--) {
        int abs_y = (Section->get_y() * 16) + y;

        for (int z = 0; z < mc::MapZ; z++) {
          for (int x = 0; x < mc::MapX; x++) {
            size_t blocked_position = x * iw + z;

            if (blocked[blocked_position]) {
              continue;
            }

            int projected_x = x;
            int projected_z = z;
            br_blocks.transform_xz(projected_x, projected_z);

            mc::BlockT block;
            if (Section->get_block(block, projected_x, projected_z, y)) {
              if (!block.material->enabled) {
                continue;
              }

              blocked[blocked_position] = block.material->top.is_opaque();

              point p(x, abs_y, z);
              pos_t px = 0;
              pos_t py = 0;
              flat_base<C>::project_position(p, px, py);

              // FIXME; this should not be used by any renderer - deprecated!
              // There is no flat engine that cares about this.
              int block_type = -1;

              switch(block.material->mode) {
              case mc::MaterialMode::Block:
              case mc::MaterialMode::LegacyLeaves:
                render_block(oper, block_type, px, py, block.material->top, block.material->side);
                render_halfblock(oper, block_type, px, py, block.material->top, block.material->side);
                break;
              case mc::MaterialMode::HalfBlock:
              case mc::MaterialMode::LegacySlab:
                render_halfblock(oper, block_type, px, py, block.material->top, block.material->side);
                break;
              case mc::MaterialMode::TorchBlock:
                render_torchblock(oper, block_type, px, py, block.material->top, block.material->side);
                break;
              case mc::MaterialMode::LargeFlowerBlock:
                if (block.properties.is_top) {
                  render_block(oper, block_type, px, py, block.material->top, block.material->side);
                  render_halfblock(oper, block_type, px, py, block.material->top, block.material->side);
                } else {
                  render_block(oper, block_type, px, py, block.material->side, block.material->side);
                  render_halfblock(oper, block_type, px, py, block.material->side, block.material->side);
                }
                break;
              case mc::MaterialMode::LogBlock:
                switch(block.properties.orientation) {
                  case mc::BlockOrientation::UpDown:
                    render_block(oper, block_type, px, py, block.material->top, block.material->side);
                    render_halfblock(oper, block_type, px, py, block.material->top, block.material->side);
                    break;
                  case mc::BlockOrientation::EastWest:
                  case mc::BlockOrientation::NorthSouth:
                    render_block(oper, block_type, px, py, block.material->side, block.material->top);
                    render_halfblock(oper, block_type, px, py, block.material->side, block.material->top);
                    break;
                  case mc::BlockOrientation::OnlySides:
                    render_block(oper, block_type, px, py, block.material->side, block.material->side);
                    render_halfblock(oper, block_type, px, py, block.material->side, block.material->side);
                    break;
                  case mc::BlockOrientation::Invalid:
                    break;
                }
                break;
              }
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
