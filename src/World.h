#ifndef _WORLD_H_
#define _WORLD_H_

#include <limits.h>

#include <string>
#include <sstream>
#include <vector>

#include "fileutils.h"
#include "global.h"

struct level {
  int xPos, zPos;
  int xReal, zReal;
};

static const char *b36alphabet = "0123456789abcdefghijklmnopqrstuvwxyz";

class World {
public:
  std::string world_path;
  std::list<level> levels;
  
  int min_x;
  int min_z;
  int max_x;
  int max_z;
  
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
  
  World(settings_t *s, std::string world_path)
    : world_path(world_path), min_x(INT_MAX), min_z(INT_MAX), max_x(INT_MIN), max_z(INT_MIN)
  {
    dirlist broadlisting(world_path);
    
    // broad phase listing of all the levels to figure out how they are ordered.
    {
      while (broadlisting.hasnext()) {
        Level leveldata(broadlisting.next().c_str(), true);
        
        if (!leveldata.islevel || leveldata.grammar_error) {
          continue;
        }
        
        if (s->use_limits && (
            leveldata.xPos < s->limits[0] ||
            leveldata.xPos > s->limits[1] ||
            leveldata.zPos < s->limits[2] ||
            leveldata.zPos > s->limits[3])
          )
        {
          continue;
        }
        
        level l;

        l.xReal = leveldata.xPos;
        l.zReal = leveldata.zPos;
        
        switch (s->rotation) {
        case 270:
          l.xPos = leveldata.zPos;
          l.zPos = -leveldata.xPos;
          break;
        case 180:
          l.xPos = -leveldata.xPos;
          l.zPos = -leveldata.zPos;
          break;
        case 90:
          l.xPos = -leveldata.zPos;
          l.zPos = leveldata.xPos;
          break;
        default:
          l.xPos = leveldata.xPos;
          l.zPos = leveldata.zPos;
          break;
        }
        
        if (l.xPos < min_x) {
          min_x = l.xPos;
        }
        
        if (l.xPos > max_x) {
          max_x = l.xPos;
        }

        if (l.zPos < min_z) {
          min_z = l.zPos;
        }
        
        if (l.zPos > max_z) {
          max_z = l.zPos;
        }
        
        levels.push_back(l);
      }
      
      levels.sort(compare_levels);
    }
  }
  
  static std::string base36(int number) {
    if (number == 0) {
      return std::string("0");
    }
    
    std::stringstream ss; 
    
    if (number < 0) {
      ss << "-";
      number = -number;
    }
    
    std::vector<char> v;

    int div, mod;
    
    while (number != 0) {
      div = number / 36;
      mod = number % 36;
      v.push_back(b36alphabet[mod]);
      number = div;
    }

    for (std::vector<char>::reverse_iterator it = v.rbegin(); it!=v.rend(); ++it) {
      ss << *it;
    }
    
    return ss.str();
  }
  
  std::string get_level_path(level &l) {
    int modx = l.xReal % 64;
    if (modx < 0) modx += 64;
    int modz = l.zReal % 64;
    if (modz < 0) modz += 64;
    return path_join(world_path, path_join(path_join(base36(modx), base36(modz)), "c." + base36(l.xReal) + "." + base36(l.zReal) + ".dat"));
  }
};

#endif /* _WORLD_H_ */
