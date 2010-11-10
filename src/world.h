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

struct world_pos {
  int x, z;
  
  bool operator<(const world_pos& o) const
  {
    if (x < o.x) return true;
    if (x == o.x && z < o.z) return true;
    return false;
  }
};

class world_info {
public:
  fs::path world_path;
  std::list<level> levels;
  
  int min_x;
  int min_z;
  int max_x;
  int max_z;
  int diff_x;
  int diff_z;
  int min_xp;
  int min_zp;
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
      
      fast_level_file leveldata(next, s.pedantic_broad_phase);
      
      if (!leveldata.islevel || leveldata.grammar_error) {
        continue;
      }
      
      if (leveldata.xPos < s.min_x ||
          leveldata.xPos > s.max_x ||
          leveldata.zPos < s.min_z ||
          leveldata.zPos > s.max_z) {
        
        if (!s.silent && s.debug) {
          std::cout << "Ignoring block out of limit range: " << leveldata.xPos << "," << leveldata.zPos << " - " << leveldata.path << std::endl;
        }
      
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
    
    diff_x = max_x - min_x;
    diff_z = max_z - min_z;
    min_xp = min_x * mc::MapX;
    min_zp = min_z * mc::MapZ;
    
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
    typedef boost::ptr_map<world_pos, std::vector<level> > world_levels_type;

    world_levels_type world_levels;
    
    for (std::list<level>::iterator it = levels.begin(); it != levels.end(); it++) {
      level l = *it;
      
      div_t d_x = div(l.xPos, chunk_size);
      div_t d_z = div(l.zPos, chunk_size);

      if (d_x.rem < 0) { d_x.rem = chunk_size + d_x.rem, --d_x.quot; }
      if (d_z.rem < 0) { d_z.rem = chunk_size + d_z.rem, --d_z.quot; }

      world_pos pos;
      pos.x = d_x.quot;
      pos.z = d_z.quot;
      
      std::vector<level>* levels;
      
      world_levels_type::iterator it = world_levels.find(pos);
      
      if (it == world_levels.end()) {
        levels = new std::vector<level>();
        world_levels.insert(pos, levels);
      }
      else {
        levels = (*it).second;
      }
      
      levels->push_back(l);
    }

    world_info** worlds = new world_info*[world_levels.size() + 1];

    int i = 0;
    
    for (world_levels_type::iterator it = world_levels.begin(); it != world_levels.end(); it++) {
      world_pos pos = (*it).first;
      std::vector<level>* levels = (*it).second;
      
      world_info *w = worlds[i++] = new world_info();
      w->min_z = 10000;
      w->max_z = -10000;
      w->min_x = 10000;
      w->max_x = -10000;
      w->world_path = world_path;
      
      w->chunk_x = pos.x;
      w->chunk_y = pos.z;
      
      std::vector<level>::iterator it = levels->begin();

      w->min_x = (pos.x) * chunk_size;
      w->max_x = (pos.x + 1) * chunk_size;
      w->min_z = (pos.z) * chunk_size;
      w->max_z = (pos.z + 1) * chunk_size;
      w->min_xp = w->min_x * mc::MapX;
      w->min_zp = w->min_z * mc::MapZ;
      
      for (; it != levels->end(); it++) {
        level l = *it;
        
        /*w->min_x = std::min(w->min_x, l.xPos);
        w->max_x = std::max(w->max_x, l.xPos);
        w->min_z = std::min(w->min_z, l.zPos);
        w->max_z = std::max(w->max_z, l.zPos);*/
        
        w->levels.push_back(l);
      }

      w->max_x -= 1;
      w->max_z -= 1;
    }
    
    worlds[world_levels.size()] = NULL;
    
    return worlds;
  }
};

#endif /* _WORLD_H_ */
