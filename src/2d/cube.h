// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _CUBE_H_
#define _CUBE_H_

#include <stdlib.h>

struct point {
  size_t x;
  size_t y;
  size_t z;

  inline point(size_t x, size_t y, size_t z) : x(x), y(y), z(z) {}
};

class point2 {
public:
  const size_t x;
  const size_t y;

  point2(const size_t x, const size_t y) : x(x), y(y) {}
  
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
  const size_t x;
  const size_t y;
  const size_t z;
  
  Cube(const size_t x, const size_t y, const size_t z) : x(x), y(y), z(z) {}
  
  void project_top(point &p, size_t &rx, size_t &ry) {
    rx = z - p.z - 1;
    ry = p.x;
  }
  
  void project_oblique(point &p, size_t &rx, size_t &ry) {
    rx = z - p.z - 1;
    ry = p.x + (y - p.y - 1);
  }
  
  void project_obliqueangle(point &p, size_t &rx, size_t &ry) {
    rx = (z - p.z - 1) + p.x;
    ry = (y - p.y - 1) + p.z + p.x;
  }

  void project_isometric(point &p, size_t &rx, size_t &ry) {
    rx = 2 * ((z - p.z - 1) + p.x);
    ry = 2 * (y - p.y - 1) + p.z + p.x;
  }
  
  void get_top_limits(size_t &rx, size_t &ry) {
    rx = z;
    ry = x;
  }
  
  void get_oblique_limits(size_t &rx, size_t &ry) {
    rx = z;
    ry = x + y;
  }
  
  void get_obliqueangle_limits(size_t &rx, size_t &ry) {
    rx = z + x;
    ry = y + z + x;
  }
  
  void get_isometric_limits(size_t &rx, size_t &ry) {
    rx = 2 * (z + x);
    ry = 2 * y + z + x;
  }
};

#endif /* _CUBE_H_ */
