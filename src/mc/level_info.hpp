#ifndef _MC_LEVEL_INFO_HPP
#define _MC_LEVEL_INFO_HPP

#include <sstream>

#include "mc/utils.hpp"

#include <boost/shared_ptr.hpp>

namespace mc {
  class level_info;
  class region;

  class level_info {
    public:
      typedef boost::shared_ptr<region> region_ptr;
      typedef boost::shared_ptr<level_info> level_info_ptr;

      level_info();
      level_info(region_ptr _region, int x, int z);
      level_info(region_ptr _region, utils::level_coord coord);

      std::string get_path();

      region_ptr get_region();
      time_t modification_time();
      
      bool operator<(const level_info& other) const;

      level_info rotate(int degrees);
      
      int get_x();
      int get_z();
      const utils::level_coord get_coord();
    private:
      region_ptr _region;
      utils::level_coord coord;
      fs::path path;
  };
}

#endif /* _MC_LEVEL_INFO_HPP */
