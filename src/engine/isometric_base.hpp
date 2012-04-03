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
    //const engine_settings& s = engine_base<C>::get_settings();
    //boost::shared_ptr<mc::Level_Compound> L = level->get_level();
  }

  virtual void render_block(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
  virtual void render_halfblock(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
  virtual void render_torchblock(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
};

#endif /* _ENGINE_ISOMETRIC_BASE_HPP */
