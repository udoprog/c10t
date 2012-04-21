#include "mc/rotated_level_info.hpp"
#include "mc/level_info.hpp"

namespace mc {
  rotated_level_info::rotated_level_info(
      level_info_ptr level,
      utils::level_coord coord,
      utils::level_coord orig
      )
      : level(level), coord(coord), nonrotated_coord(orig)
  {
  }
    
  bool rotated_level_info::operator<(const rotated_level_info& other) const {
    return coord < other.coord;
  }

  rotated_level_info::level_info_ptr rotated_level_info::get_level()
  {
    return level;
  }

  utils::level_coord rotated_level_info::get_coord()
  {
    return coord;
  }
 
  utils::level_coord rotated_level_info::get_original_coord()
  {
    return nonrotated_coord;
  } 
}
