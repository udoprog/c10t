#include "engine/topdown_engine.hpp"

topdown_engine::topdown_engine(
    engine_settings& s,
    mc::world& world
    )
  : flat_base<top_cube>(s, world)
{
}

void topdown_engine::render_block(
    image_operations_ptr o,
    int bt,
    pos_t px,
    pos_t py,
    color top,
    color side
    )
{
  o->add_pixel(px, py, top);
}

void topdown_engine::render_halfblock(
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

void topdown_engine::render_torchblock(
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
