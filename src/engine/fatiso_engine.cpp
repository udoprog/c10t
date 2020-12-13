#include "engine/fatiso_engine.hpp"

fatiso_engine::fatiso_engine(
    engine_settings& s,
    mc::world& world
    )
  : isometric_base<fatiso_cube>(s, world)
{
}

void fatiso_engine::render_block(
    image_operations_ptr o,
    int bt,
    pos_t px,
    pos_t py,
    color top,
    color side
    )
{
  color topdark(top);
  color toplight(top);
  color sidelight(side);

  if (bt == mc::LegacyBlocks::Grass) {
    topdark.darken(0x20);
    toplight.darken(0x10);
    sidelight.lighten(0x20);
  }
  else {
    toplight = color(side);
    topdark = color(side);
  }

  o->add_pixel(px + 0, py + 0, side);
  o->add_pixel(px + 0, py + 1, side);
  o->add_pixel(px + 0, py + 2, side);
  o->add_pixel(px + 1, py - 1, side);
  o->add_pixel(px + 1, py + 0, side);
  o->add_pixel(px + 1, py + 1, side);
  o->add_pixel(px + 2, py - 1, side);
  o->add_pixel(px + 2, py + 0, side);
  o->add_pixel(px + 2, py + 1, side);
  o->add_pixel(px + 3, py - 2, side);
  o->add_pixel(px + 3, py - 1, side);
  o->add_pixel(px + 3, py + 0, side);

  o->add_pixel(px - 1, py + 0, sidelight);
  o->add_pixel(px - 1, py + 1, sidelight);
  o->add_pixel(px - 1, py + 2, sidelight);
  o->add_pixel(px - 2, py - 1, sidelight);
  o->add_pixel(px - 2, py + 0, sidelight);
  o->add_pixel(px - 2, py + 1, sidelight);
  o->add_pixel(px - 3, py - 1, sidelight);
  o->add_pixel(px - 3, py + 0, sidelight);
  o->add_pixel(px - 3, py + 1, sidelight);
  o->add_pixel(px - 4, py - 2, sidelight);
  o->add_pixel(px - 4, py - 1, sidelight);
  o->add_pixel(px - 4, py + 0, sidelight);

  o->add_pixel(px + 0, py - 2, topdark);
  o->add_pixel(px + 0, py - 1, topdark);
  o->add_pixel(px + 1, py - 3, topdark);
  o->add_pixel(px + 1, py - 2, topdark);
  o->add_pixel(px + 2, py - 3, topdark);
  o->add_pixel(px + 2, py - 2, topdark);
  o->add_pixel(px + 3, py - 4, topdark);
  o->add_pixel(px + 3, py - 3, topdark);

  o->add_pixel(px - 1, py - 2, toplight);
  o->add_pixel(px - 1, py - 1, toplight);
  o->add_pixel(px - 2, py - 3, toplight);
  o->add_pixel(px - 2, py - 2, toplight);
  o->add_pixel(px - 3, py - 3, toplight);
  o->add_pixel(px - 3, py - 2, toplight);
  o->add_pixel(px - 4, py - 4, toplight);
  o->add_pixel(px - 4, py - 3, toplight);

  o->add_pixel(px - 3, py - 5, top);
  o->add_pixel(px - 3, py - 4, top);
  o->add_pixel(px - 2, py - 5, top);
  o->add_pixel(px - 2, py - 4, top);
  o->add_pixel(px - 1, py - 6, top);
  o->add_pixel(px - 1, py - 5, top);
  o->add_pixel(px - 1, py - 4, top);
  o->add_pixel(px - 1, py - 3, top);
  o->add_pixel(px + 0, py - 6, top);
  o->add_pixel(px + 0, py - 5, top);
  o->add_pixel(px + 0, py - 4, top);
  o->add_pixel(px + 0, py - 3, top);
  o->add_pixel(px + 1, py - 5, top);
  o->add_pixel(px + 1, py - 4, top);
  o->add_pixel(px + 2, py - 5, top);
  o->add_pixel(px + 2, py - 4, top);
}

void fatiso_engine::render_halfblock(
    image_operations_ptr o,
    int bt,
    pos_t px,
    pos_t py,
    color top,
    color side
    )
{
  render_block(o, bt, px, py, top, side);
}

void fatiso_engine::render_torchblock(
    image_operations_ptr o,
    int bt,
    pos_t px,
    pos_t py,
    color top,
    color side
    )
{
  render_block(o, bt, px, py, top, side);
}
