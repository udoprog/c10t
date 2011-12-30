#ifndef _ENGINE_FUNCTIONS_HPP
#define _ENGINE_FUNCTIONS_HPP

#include "engine/block_rotation.hpp"
#include "engine/engine_settings.hpp"
#include "image/color.hpp"
#include "mc/blocks.hpp"

/**
 * Shared functions between engines.
 */

/**
 * Apply shading to specified block and color.
 **/
void apply_shading(
    const engine_settings& engine_s,
    int bl, int sl, int hm, int y, color &c);

inline bool is_open(int bt) {
  if (bt == -1) {
    return false;
  }
  
  switch(bt) {
    case mc::Air: return true;
    case mc::Leaves: return true;
    default: return false;
  }
}

inline bool cave_ignore_block(int y, int bt, block_rotation& b_r, bool &cave_initial) {
  if (cave_initial) {
    if (!is_open(bt)) {
      cave_initial = false;
      return true;
    }
    
    return true;
  }
  
  if (!is_open(bt) && is_open(b_r.get8(y + 1))) {
    return false;
  }
  
  return true;
}

inline bool hell_ignore_block(int y, int bt, block_rotation& b_r, bool &hell_initial) {
  if (hell_initial) {
    if (is_open(bt)) {
      hell_initial = false;
      return false;
    }
    
    return true;
  }
  
  return false;
}

inline color blockColor_top(int blockType, int y, block_rotation blockData) {
  return mc::getColor(blockType, blockData.get4(y));
}

inline color blockColor_side(int blockType, int y, block_rotation blockData) {
  return mc::getSideColor(blockType, blockData.get4(y));
}

#endif /* _ENGINE_FUNCTIONS_HPP */
