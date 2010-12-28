// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef __MC_WORLD_HPP__
#define __MC_WORLD_HPP__

#include <stdlib.h>
#include <limits.h>

#include <string>
#include <queue>

#include "fileutils.hpp"

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/filesystem.hpp>

#include <mc/utils.hpp>

namespace mc {
  namespace fs = boost::filesystem;
  
  bool directory_filter(const std::string& name);
  bool file_filter(const std::string& name);
  
  struct level_info {
    private:
      const fs::path& root;
      const utils::level_coord coord;
    public:
      level_info(const fs::path& root, int x, int z) : root(root), coord(x, z) {
      }
      
      level_info(const fs::path& root, utils::level_coord coord) : root(root), coord(coord) {
      }
      
      const fs::path& get_root() {
        return root;
      }
      
      const fs::path get_path() {
        return mc::utils::level_path(root, coord.get_x(), coord.get_z(), "c", "dat");
      }
      
      bool operator<(const level_info& other) const {
        return coord < other.coord;
      }

      level_info rotate(int degrees) {
        return level_info(root, coord.rotate(degrees));
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
    public:
      chunk_iterator(const fs::path path) : root(path), lister(path) {
      }
      
      bool has_next() {
        return lister.has_next(directory_filter, file_filter);
      }
      
      level_info next() {
        fs::path next = lister.next();

        try {
          mc::utils::level_coord coord = mc::utils::path_to_level_coord(next);
          return level_info(root, coord.get_x(), coord.get_z());
        } catch(std::exception& e) {
          throw bad_level(next, e.what());
        }
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
