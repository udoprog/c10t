// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _WORLD_H_
#define _WORLD_H_

#include <limits.h>

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>

#include <boost/filesystem.hpp>

#include "fileutils.h"
#include "global.h"
#include "common.h"
#include "level.h"

namespace fs = boost::filesystem;

void transform_world_xz(int& x, int& z, int rotation);

struct level {
  int xPos, zPos;
  int xReal, zReal;
};

class world_info {
public:
  fs::path world_path;
  std::list<level> levels;
  
  int min_x;
  int min_z;
  int max_x;
  int max_z;
  int chunk_x;
  int chunk_y;
  
  static bool compare_levels(level first, level second)
  {
    if (first.zPos < second.zPos) {
      return true;
    }
    
    if (first.zPos > second.zPos) {
      return false;
    }
    
    return first.xPos < second.xPos;;
  }

  world_info() {
  }
  
  world_info(settings_t& s, fs::path world_path, void (*c_progress)(int, int))
    : world_path(world_path), min_x(INT_MAX), min_z(INT_MAX), max_x(INT_MIN), max_z(INT_MIN), chunk_x(0), chunk_y(0)
  {
    dirlist broadlisting(world_path);

    int i = 1;
    
    // broad phase listing of all the levels to figure out how they are ordered.
    while (broadlisting.has_next()) {
      if (c_progress != NULL) c_progress(i++, 0);
      
      fs::path next = broadlisting.next();
      
      fast_level_file leveldata(next, !s.pedantic_broad_phase);
      
      if (!leveldata.islevel || leveldata.grammar_error) {
        continue;
      }

      if (leveldata.xPos < s.min_x ||
          leveldata.xPos > s.max_x ||
          leveldata.zPos < s.min_z ||
          leveldata.zPos > s.max_z) {
       continue;
      }

      level l;

      l.xReal = leveldata.xPos;
      l.zReal = leveldata.zPos;

      l.xPos = leveldata.xPos;
      l.zPos = leveldata.zPos;
      
      transform_world_xz(l.xPos, l.zPos, s.rotation);

      min_x = std::min(min_x, l.xPos);
      max_x = std::max(max_x, l.xPos);
      min_z = std::min(min_z, l.zPos);
      max_z = std::max(max_z, l.zPos);

      levels.push_back(l);
    }

    levels.sort(compare_levels);
    
    if (c_progress != NULL) c_progress(i++, 1);
  }
  
  fs::path get_level_path(level &l) {
    using common::b36encode;
    int modx = l.xReal % 64;
    if (modx < 0) modx += 64;
    int modz = l.zReal % 64;
    if (modz < 0) modz += 64;
    return world_path / b36encode(modx) / b36encode(modz) / ("c." + b36encode(l.xReal) + "." + b36encode(l.zReal) + ".dat");
  }

  world_info** split(int chunk_size) {
    int b_min_z = min_z / chunk_size;
    if (min_z % chunk_size != 0) b_min_z -= 1;
    int b_min_x = min_x / chunk_size;
    if (min_x % chunk_size != 0) b_min_x -= 1;
    int b_max_z = max_z / chunk_size;
    if (max_z % chunk_size != 0) b_max_z += 1;
    int b_max_x = max_x / chunk_size;
    if (max_x % chunk_size != 0) b_max_x += 1;
    
    int b_diff_z = b_max_z - b_min_z;
    int b_diff_x = b_max_x - b_min_x;
    
    int world_count = b_diff_z * b_diff_x;

    world_info** worlds = new world_info*[world_count + 1];
    
    for (int z = 0; z < b_diff_z; z++) {
      for (int x = 0; x < b_diff_x; x++) {
        int p = z + (x * b_diff_z);
        world_info *w = worlds[p] = new world_info();
        w->min_z = (z + b_min_z) * chunk_size;
        w->max_z = (z + b_min_z + 1) * chunk_size - 1;
        w->min_x = (x + b_min_x) * chunk_size;
        w->max_x = (x + b_min_x + 1) * chunk_size - 1;
        w->world_path = fs::path(world_path);
        w->chunk_x = b_diff_z - z - 1;
        w->chunk_y = x;
        //std::cout << w->min_z << " " << w->max_z << " | " << w->min_x << " " << w->max_x << std::endl;
      }
    }

    for (std::list<level>::iterator it = levels.begin(); it != levels.end(); it++) {
      level l = *it;
      int l_z, l_x;

      if (l.zPos < 0) {
        l_z = l.zPos / chunk_size - 1;
        if (l.zPos % chunk_size == 0) l_z += 1;
      }
      else {
        l_z = l.zPos / chunk_size;
      }
      
      if (l.xPos < 0) {
        l_x = l.xPos / chunk_size - 1;
        if (l.xPos % chunk_size == 0) l_x += 1;
      }
      else {
        l_x = l.xPos / chunk_size;
      }

      int z = l_z - b_min_z;
      int x = l_x - b_min_x;
      
      world_info* w = worlds[z + (x * b_diff_z)];
      
      //std::cout << x << ":" << z << std::endl;
      //std::cout << l.zPos << ":" << l_z << std::endl;
      //std::cout << l.xPos << ":" << l_x << std::endl;
      w->levels.push_back(l);
      //std::cout << "(" << l_block_z << " " << l_block_x << ")" << std::endl;
    }
    
    //std::cout << min_z << " " << max_z << " - " << b_min_z << " " << b_max_z << " (" << b_diff_z << ")" << std::endl;
    //std::cout << min_x << " " << max_x << " - " << b_min_x << " " << b_max_x << " (" << b_diff_x << ")" << std::endl;
    worlds[world_count] = NULL;
    
    return worlds;
  }
};

#endif /* _WORLD_H_ */
