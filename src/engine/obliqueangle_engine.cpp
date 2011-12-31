#include "engine/obliqueangle_engine.hpp"

obliqueangle_engine::obliqueangle_engine(
    engine_settings& s,
    mc::world& world
    )
  : isometric_base(s, world)
{
}

void obliqueangle_engine::render_block(
    image_operations_ptr o,
    int bt,
    pos_t px,
    pos_t py,
    color top,
    color side
    )
{
  o->add_pixel(px, py, top);
  o->add_pixel(px + 1, py, top);
  o->add_pixel(px, py + 1, side);
  
  side.lighten(0x20);
  o->add_pixel(px + 1, py + 1, side);
}

void obliqueangle_engine::render_halfblock(
    image_operations_ptr o,
    int bt,
    pos_t px,
    pos_t py,
    color top,
    color side
    )
{
  o->add_pixel(px, py + 1, top);
  o->add_pixel(px + 1, py + 1, top);
}

void obliqueangle_engine::render_torchblock(
    image_operations_ptr o,
    int bt,
    pos_t px,
    pos_t py,
    color top,
    color side
    )
{
  o->add_pixel(px, py, top);

  top.lighten(0x20);
  top.a -= 0xb0;
  o->add_pixel(px - 1, py, top);
  o->add_pixel(px + 2, py, top);
  o->add_pixel(px, py - 1, top);
  o->add_pixel(px, py + 1, top);
  
  o->add_pixel(px, py + 1, side);
}
