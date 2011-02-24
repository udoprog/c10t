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
  
  bool directory_filter(const std::string& name);
  bool file_filter(const std::string& name);
  
  struct level_info {
    public:
      typedef boost::shared_ptr<region> region_ptr;
    private:
      region_ptr _region;
      utils::level_coord coord;
    public:
      level_info() : coord() {
      }

      level_info(region_ptr _region, int x, int z) : _region(_region), coord(x, z) {
      }
      
      level_info(region_ptr _region, utils::level_coord coord) : _region(_region) {
        utils::level_coord rc = utils::path_to_region_coord(_region->get_path());
        this->coord = utils::level_coord(rc.get_x() + coord.get_x(),
                                         rc.get_z() + coord.get_z());
      }

      std::string get_path() {
        std::stringstream ss;
        ss << _region->get_path() << "(" << coord.get_x() << "," << coord.get_z() << ")";
        return ss.str();
      }

      region_ptr get_region() {
        return _region;
      }
      
      bool operator<(const level_info& other) const {
        return coord < other.coord;
      }

      level_info rotate(int degrees) {
        return level_info(_region, coord.rotate(degrees));
      }
      
      int get_x() { return coord.get_x(); }
      int get_z() { return coord.get_z(); }
      const utils::level_coord get_coord() { return coord; }
  };
  
  class bad_level : public std::exception {
    private:
      const fs::path path;
      const char* message;
    public:
      bad_level(const fs::path path, const char* message) 
        : path(path), message(message)
      {
        
      }

      ~bad_level() throw() {  }

      const char* what() const throw() {
        return message;
      }
      
      const fs::path where() {
        return path;
      }
  };
  
  class chunk_iterator {
    private:
      fs::path root;
      dirlist lister;
      std::list<utils::level_coord> current_region;
      boost::shared_ptr<region> current_region_r;
    public:
      chunk_iterator(const fs::path path) : root(path), lister(path) {
      }
      
      bool has_next() {
        if (current_region.size() > 0) {
          return true;
        }

        if (!lister.has_next(directory_filter, file_filter)) {
          return false;
        }

        fs::path next = lister.next();
        current_region_r.reset(new region(next));

        current_region_r->read_coords(current_region);

        if (current_region.size() > 0) {
          return true;
        }

        return false;
      }
      
      boost::shared_ptr<level_info> next() {
        if (current_region.size() > 0) {
          utils::level_coord c = current_region.front();
          current_region.pop_front();

          try {
            boost::shared_ptr<level_info> ptr(new level_info(current_region_r, c));
            return ptr;
          } catch(std::exception& e) {
            throw bad_level(current_region_r->get_path(), e.what());
          }
        }

        throw bad_level("", "No more regions");
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
    chunk_iterator get_iterator();
    void update(utils::level_coord coord);
  };
}

#endif /* __MC_WORLD_HPP__ */
