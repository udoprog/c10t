// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "world.hpp"

#include <mc/utils.hpp>
#include <mc/blocks.hpp>

#include <iostream>

namespace mc {
  bool directory_filter(const std::string& name)
  {
    if (name.compare(0, 3, "DIM") == 0) return false;
    if (name.compare("players") == 0) return false;
    return true;
  }
  
  bool file_filter(const std::string& name) {
    if (name.length() < 8) return false;
    if (name.compare(name.length() - 4, 4, ".mcr") != 0) return false;
    if (name.compare(0, 2, "r.") != 0) return false;
    return true;
  }

  world::world(fs::path world_path)
    : world_path(world_path), min_x(INT_MAX), min_z(INT_MAX), max_x(INT_MIN), max_z(INT_MIN), chunk_x(0), chunk_y(0)
  { }
  
  region_iterator world::get_iterator() {
    return region_iterator(world_path);
  }

  void world::update(utils::level_coord coord) {
    min_x = std::min(min_x, coord.get_x());
    max_x = std::max(max_x, coord.get_x());
    min_z = std::min(min_z, coord.get_z());
    max_z = std::max(max_z, coord.get_z());
    
    diff_x = max_x - min_x;
    diff_z = max_z - min_z;
    min_xp = min_x * mc::MapX;
    min_zp = min_z * mc::MapZ;
  }
}
