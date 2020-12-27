#ifndef _ENGINE_ISOMETRIC_BASE_HPP
#define _ENGINE_ISOMETRIC_BASE_HPP

#include "engine/engine_base.hpp"
#include "engine/block_rotation.hpp"
#include "engine/functions.hpp"

#include <boost/foreach.hpp>

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

    boost::shared_ptr<mc::Level_Compound> L = level->get_level();

    pos_t image_width = 0;
    pos_t image_height = 0;

    engine_base<C>::project_limits(image_width, image_height);

    oper->set_limits(image_width + 1, image_height);

    BOOST_FOREACH(boost::shared_ptr<mc::Section_Compound> Section, L->Sections) {
      block_rotation br_blocks(s.rotation);

      for (int y = 0; y < 16; y++) {
        int abs_y = (16 * Section->get_y()) + y;

        for (int z = 0; z < mc::MapZ; z++) {
          for (int x = mc::MapX - 1; x >= 0; x--) {
            int projected_x = x;
            int projected_z = z;
            br_blocks.transform_xz(projected_x, projected_z);

            mc::BlockT block;
            if (Section->get_block(block, projected_x, projected_z, y)) {
              if (!block.material->enabled) {
                continue;
              }

              point p(x, abs_y, z);
              pos_t px = 0;
              pos_t py = 0;
              engine_base<C>::project_position(p, px, py);

              // FIXME; this should not be used by any renderer - deprecated!
              // The fatiso engine cares; there is a special handler for grass
              // so try to countine passing this along for blocks with known
              // leagacy id(s).
              int block_type;
              if (block.material->legacy_ids.size() > 0) {
                block_type = block.material->legacy_ids[0];
              } else {
                block_type = -1;
              }

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
                    // TODO: Actually implement render rotation, for now simply swap top and side.
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
  }

  virtual void render_block(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
  virtual void render_halfblock(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
  virtual void render_torchblock(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
};

#endif /* _ENGINE_ISOMETRIC_BASE_HPP */
