#include "engine/isometric_engine.hpp"

isometric_engine::isometric_engine(
    engine_settings& s,
    mc::world& world
    )
  : isometric_base(s, world)
{
}

void isometric_engine::render_block(
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
  o->add_pixel(px - 2, py, top);
  o->add_pixel(px - 1, py, top);
  
  o->add_pixel(px - 2, py + 1, side);
  
  o->add_pixel(px - 1, py + 1, side);
  o->add_pixel(px - 1, py + 2, side);
  
  o->add_pixel(px - 2, py + 2, side);
  
  side.lighten(0x20);
  
  o->add_pixel(px, py + 1, side);
  o->add_pixel(px, py + 2, side);
  
  o->add_pixel(px + 1, py + 1, side);
  o->add_pixel(px + 1, py + 2, side);
}

void isometric_engine::render_halfblock(
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
  o->add_pixel(px - 2, py + 1, top);
  o->add_pixel(px - 1, py + 1, top);
  
  o->add_pixel(px - 2, py + 2, side);
  o->add_pixel(px - 1, py + 2, side);
  
  side.lighten(0x20);
  
  o->add_pixel(px, py + 2, side);
  o->add_pixel(px + 1, py + 2, side);
}

void isometric_engine::render_torchblock(
    image_operations_ptr o,
    int bt,
    pos_t px,
    pos_t py,
    color top,
    color side
    )
{
  o->add_pixel(px, py, top);
  o->add_pixel(px - 1, py, top);
  
  top.lighten(0x20);
  top.a -= 0xb0;
  
  o->add_pixel(px, py + 1, top);
  o->add_pixel(px - 1, py + 1, top);
  
  o->add_pixel(px - 1, py + 1, side);
  o->add_pixel(px - 1, py + 2, side);
  
  side.lighten(0x20);
  
  o->add_pixel(px, py + 1, side);
  o->add_pixel(px, py + 2, side);
  
  o->add_pixel(px - 2, py, top);
  o->add_pixel(px + 1, py, top);
  o->add_pixel(px, py - 1, top);
  o->add_pixel(px - 1, py - 1, top);
}
