#ifndef _MC_ROTATED_LEVEL_INFO_HPP
#define _MC_ROTATED_LEVEL_INFO_HPP

#include <boost/shared_ptr.hpp>

#include "mc/utils.hpp"

namespace mc {
  class level_info;

  class rotated_level_info {
  public:
    typedef boost::shared_ptr<level_info> level_info_ptr;
    
    rotated_level_info(level_info_ptr level, utils::level_coord coord, utils::level_coord nonrotated);
    bool operator<(const rotated_level_info& other) const;
    level_info_ptr get_level();
    utils::level_coord get_coord();
    utils::level_coord get_original_coord();

  private:
    level_info_ptr level;
    mc::utils::level_coord coord;
    mc::utils::level_coord nonrotated_coord;
  };
}

#endif /* _MC_ROTATED_LEVEL_INFO_HPP */
