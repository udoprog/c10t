// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "world.hpp"

void transform_world_xz(int& x, int& z, int rotation)
{
  int t = x;
  switch (rotation) {
    case 270:
      x = z;
      z = -t;
      break;
    case 180:
      x = -x;
      z = -z;
      break;
    case 90:
      x = -z;
      z = t;
      break;
  }
}

inline bool directory_filter(const std::string& name)
{
  if (name.compare(0, 3, "DIM") == 0) return true;
  if (name.compare("players") == 0) return true;
  return false;
}

bool compare_levels(level first, level second)
{
  if (first.zPos < second.zPos) {
    return true;
  }
  
  if (first.zPos > second.zPos) {
    return false;
  }
  
  return first.xPos < second.xPos;;
}

  world_info::world_info(settings_t& s, fs::path world_path, nonstd::reporting<unsigned int>& c)
  : world_path(world_path), min_x(INT_MAX), min_z(INT_MAX), max_x(INT_MIN), max_z(INT_MIN), chunk_x(0), chunk_y(0)
{
  dirlist broadlisting(world_path);
  
  unsigned int parts = 0;
  
  // broad phase listing of all the levels to figure out how they are ordered.
  while (broadlisting.has_next(directory_filter)) {
    fs::path next = broadlisting.next();
    
    fast_level_file leveldata(next);
    
    if (!leveldata.is_level) {
      if (!s.silent && s.debug) {
        std::cout << leveldata.path << ": " << leveldata.is_level_why << std::endl;
      }
      
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

    uint64_t x2 = leveldata.xPos * leveldata.xPos;
    uint64_t z2 = leveldata.zPos * leveldata.zPos;
    uint64_t r2 = s.max_radius * s.max_radius;
    
    if (x2 + z2 >= r2) {
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
    
    ++parts;
    c.add(1);
  }
  
  c.done(0);
  levels.sort(compare_levels);
  
  diff_x = max_x - min_x;
  diff_z = max_z - min_z;
  min_xp = min_x * mc::MapX;
  min_zp = min_z * mc::MapZ;
}

fs::path world_info::get_level_path(level &l) {
  return mcutils::level_path(world_path, l.xReal, l.zReal, "c", "dat");
}

// this method is so complex, broken, whatever...
// I'm just keeping it as a reminder of how not to do this.
std::vector<world_info> world_info::split(int chunk_size)
{
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

  std::vector<world_info> worlds;
  
  /*
  for (world_levels_type::iterator it = world_levels.begin(); it != world_levels.end(); it++) {
    world_pos pos = (*it).first;
    std::vector<level>* levels = (*it).second;
    
    world_info w;

    w.min_z = 10000;
    w.max_z = -10000;
    w.min_x = 10000;
    w.max_x = -10000;
    w.world_path = world_path;
    
    w.chunk_x = pos.x;
    w.chunk_y = pos.z;
    
    std::vector<level>::iterator it = levels->begin();

    w.min_x = (pos.x) * chunk_size;
    w.max_x = (pos.x + 1) * chunk_size;
    w.min_z = (pos.z) * chunk_size;
    w.max_z = (pos.z + 1) * chunk_size;
    w.min_xp = w.min_x * mc::MapX;
    w.min_zp = w.min_z * mc::MapZ;
    
    w.levels.insert(w.levels.begin(), levels->begin(), levels->end());
    
    w.max_x -= 1;
    w.max_z -= 1;
  }
  */
  
  return worlds;
}
