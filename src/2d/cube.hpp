// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _CUBE_H_
#define _CUBE_H_

#include <stdlib.h>
#include <stdint.h>

struct point {
  typedef uint64_t pos_t;
  
  pos_t x;
  pos_t y;
  pos_t z;

  inline point(pos_t x, pos_t y, pos_t z) : x(x), y(y), z(z) {}
};

class point2 {
public:
  typedef uint64_t pos_t;
  
  const pos_t x;
  const pos_t y;

  point2(const pos_t x, const pos_t y) : x(x), y(y) {}
  
  bool operator<(const point2& oth) const {
    if (y < oth.y) {
      return true;
    }
    
    if (y == oth.y && x < oth.x) {
      return true;
    }
    
    return false;
  }
};

class Cube {
public:
  typedef uint64_t pos_t;
  
  const pos_t x;
  const pos_t y;
  const pos_t z;
  
  Cube(const pos_t x, const pos_t y, const pos_t z) : x(x), y(y), z(z) {}
  
  inline void project_top(point &p, pos_t &rx, pos_t &ry) const
  {
    rx = z - p.z - 1;
    ry = p.x;
  }
  
  inline void project_oblique(point &p, pos_t &rx, pos_t &ry) const
  {
    rx = z - p.z - 1;
    ry = p.x + (y - p.y - 1);
  }
  
  inline void project_obliqueangle(point &p, pos_t &rx, pos_t &ry) const
  {
    rx = (z - p.z - 1) + p.x;
    ry = (y - p.y - 1) + p.z + p.x;
  }

  inline void project_isometric(point &p, pos_t &rx, pos_t &ry) const
  {
    rx = 2 * ((z - p.z - 1) + p.x);
    ry = 2 * (y - p.y - 1) + p.z + p.x;
  }
  
  inline void project_fatiso(point &p, pos_t &rx, pos_t &ry) const
  {
    rx = 4 * ((z - p.z - 1) + p.x);
    ry = 5 * (y - p.y - 1) + 2 * p.z + 2 * p.x;
  }

  inline void get_top_limits(pos_t &rx, pos_t &ry) const
  {
    rx = z;
    ry = x;
  }
  
  inline void get_oblique_limits(pos_t &rx, pos_t &ry) const
  {
    rx = z;
    ry = x + y;
  }
  
  inline void get_obliqueangle_limits(pos_t &rx, pos_t &ry) const
  {
    rx = z + x;
    ry = y + z + x;
  }
  
  inline void get_isometric_limits(pos_t &rx, pos_t &ry) const
  {
    rx = 2 * (z + x);
    ry = 2 * y + z + x;
  }

  inline void get_fatiso_limits(pos_t &rx, pos_t &ry) const
  {
    rx = 4 * (z + x);
    ry = 5 * y + 2 * z + 2 * x;
  }
};

#endif /* _CUBE_H_ */
