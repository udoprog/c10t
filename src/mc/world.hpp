// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef __MC_WORLD_HPP__
#define __MC_WORLD_HPP__

#include <stdlib.h>
#include <limits.h>

#include <string>
#include <queue>
#include <list>

#include "fileutils.hpp"

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/filesystem.hpp>

#include <mc/utils.hpp>
#include <mc/region.hpp>

namespace mc {
  namespace fs = boost::filesystem;

  class level_info;
  class region_iterator;
  
  class iterator_error : public std::exception {
    private:
      const char* message;
    public:
      iterator_error(const char* message) 
        : message(message)
      {
      }

      ~iterator_error() throw() {  }

      const char* what() const throw() {
        return message;
      }
  };
  
  class world {
  public:
    fs::path world_path;
    
    int min_x, min_z, max_x, max_z;
    // the difference between min_* and max_*
    int diff_x, diff_z;
    // min_* as a point
    int min_xp, min_zp;
    int chunk_x, chunk_y;
    
    world(fs::path path);
    region_iterator get_iterator();
    void update(utils::level_coord coord);
  };
}

#endif /* __MC_WORLD_HPP__ */
