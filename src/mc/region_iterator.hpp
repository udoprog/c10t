#ifndef _MC_REGION_ITERATOR_HPP
#define _MC_REGION_ITERATOR_HPP

#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

class dirlist;

namespace fs = boost::filesystem;

namespace mc {
  class level_info;
  class region;

  /**
   * Dynamically iterates over, and instantiates regions.
   */
  class region_iterator {
    public:
      typedef boost::shared_ptr<region> region_ptr;
      typedef boost::shared_ptr<level_info> level_info_ptr;

      region_iterator(const fs::path path);
      bool has_next();
      boost::shared_ptr<region> next();
    private:
      fs::path root;
      boost::shared_ptr<dirlist> lister;
      std::list<level_info_ptr> current_levels;
      region_ptr current_region;
  };
}

#endif /* _MC_REGION_ITERATOR_HPP */
