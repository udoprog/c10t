// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _WORLD_H_
#define _WORLD_H_

#include <stdlib.h>
#include <limits.h>

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include "fileutils.hpp"
#include "global.hpp"
#include "level.hpp"
#include "algorithm.hpp"

namespace fs = boost::filesystem;

void transform_world_xz(int& x, int& z, int rotation);

struct level {
  int xPos, zPos;
  int xReal, zReal;
};

struct world_pos {
  int x, z;
  
  bool operator<(const world_pos& o) const
  {
    if (x < o.x) return true;
    if (x == o.x && z < o.z) return true;
    return false;
  }
};

bool compare_levels(level first, level second);

inline bool directory_filter(const std::string& name);

class world_info {
public:
  fs::path world_path;
  std::list<level> levels;
  
  int min_x, min_z, max_x, max_z;
  // the difference between min_* and max_*
  int diff_x, diff_z;
  // min_* as a point
  int min_xp, min_zp;
  int chunk_x, chunk_y;
  
  world_info(settings_t& s, fs::path world_path, nonstd::reporting<unsigned int>& c);
  
  /**
   * Sugar function, uses mcutils::level_path with world_path as base
   */
  fs::path get_level_path(level &l);

  // DEPRECATED, this type of split is not good
  std::vector<world_info> split(int chunk_size);
};

#endif /* _WORLD_H_ */
