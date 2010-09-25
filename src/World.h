#ifndef _WORLD_H_
#define _WORLD_H_

#include <limits.h>

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "fileutils.h"
#include "global.h"

typedef boost::shared_ptr<Level> LevelPtr;

class World {
public:
  std::string world_path;
  std::list<LevelPtr> levels;
  
  int min_x;
  int min_z;
  int max_x;
  int max_z;
  int chunk_x;
  int chunk_y;
  
  static bool compare_levels(const LevelPtr& first, const LevelPtr& second)
  {
    if (first->zPos < second->zPos) {
      return true;
    }
    
    if (first->zPos > second->zPos) {
      return false;
    }
    
    return first->xPos < second->xPos;
  }

  World() {
  }
  
  World(settings_t& s, const std::string& world_path)
    : world_path(world_path), min_x(INT_MAX), min_z(INT_MAX), max_x(INT_MIN), max_z(INT_MIN), chunk_x(0), chunk_y(0)
  {
    dirlist broadlisting(world_path);
    
    // broad phase listing of all the levels to figure out how they are ordered.
    while (broadlisting.hasnext()) {
      LevelPtr l(new Level(s, broadlisting.next()));

      if (!l->islevel ||
          l->grammar_error ||
          l->xPos < s.min_x ||
          l->xPos > s.max_z ||
          l->zPos < s.min_z ||
          l->zPos > s.max_z) {
        continue;
      }

      min_x = std::min(min_x, l->xPos);
      max_x = std::max(max_x, l->xPos);
      min_z = std::min(min_z, l->zPos);
      max_z = std::max(max_z, l->zPos);

      levels.push_back(l);
    }

    levels.sort(compare_levels);
  }

  World** split(int chunk_size) {
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

    World** worlds = new World*[world_count + 1];
    
    for (int z = 0; z < b_diff_z; z++) {
      for (int x = 0; x < b_diff_x; x++) {
        int p = z + (x * b_diff_z);
        World *w = worlds[p] = new World();
        w->min_z = (z + b_min_z) * chunk_size;
        w->max_z = (z + b_min_z + 1) * chunk_size - 1;
        w->min_x = (x + b_min_x) * chunk_size;
        w->max_x = (x + b_min_x + 1) * chunk_size - 1;
        w->world_path = world_path;
        w->chunk_x = b_diff_z - z - 1;
        w->chunk_y = x;
        //std::cout << w->min_z << " " << w->max_z << " | " << w->min_x << " " << w->max_x << std::endl;
      }
    }

    for (std::list<LevelPtr>::iterator it = levels.begin(); it != levels.end(); it++) {
      LevelPtr& l = *it;
      int l_z, l_x;

      if (l->zPos < 0) {
        l_z = l->zPos / chunk_size - 1;
        if (l->zPos % chunk_size == 0) l_z += 1;
      }
      else {
        l_z = l->zPos / chunk_size;
      }
      
      if (l->xPos < 0) {
        l_x = l->xPos / chunk_size - 1;
        if (l->xPos % chunk_size == 0) l_x += 1;
      }
      else {
        l_x = l->xPos / chunk_size;
      }

      int z = l_z - b_min_z;
      int x = l_x - b_min_x;
      
      World* w = worlds[z + (x * b_diff_z)];
      
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
