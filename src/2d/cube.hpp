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

class top_cube {
public:
  typedef uint64_t pos_t;
  
  top_cube(const pos_t x, const pos_t y, const pos_t z) : x(x), y(y), z(z) {}
  
  inline void project(point &p, pos_t &rx, pos_t &ry) const
  {
    rx = z - p.z - 1;
    ry = p.x;
  }

  inline void limits(pos_t &rx, pos_t &ry) const
  {
    rx = z;
    ry = x;
  }
private:
  const pos_t x;
  const pos_t y;
  const pos_t z;
};

class oblique_cube {
public:
  typedef uint64_t pos_t;

  oblique_cube(const pos_t x, const pos_t y, const pos_t z) : x(x), y(y), z(z) {}

  inline void project(point &p, pos_t &rx, pos_t &ry) const
  {
    rx = z - p.z - 1;
    ry = p.x + (y - p.y - 1);
  }

  inline void limits(pos_t &rx, pos_t &ry) const
  {
    rx = z;
    ry = x + y;
  }
private:
  const pos_t x;
  const pos_t y;
  const pos_t z;
};

template<int F1 = 1, int F2 = 1, int F3 = 1>
class angle_cube {
public:
  typedef uint64_t pos_t;

  angle_cube(const pos_t x, const pos_t y, const pos_t z) : x(x), y(y), z(z) {}
  
  inline void project(point &p, pos_t &rx, pos_t &ry) const
  {
    rx = F1 * ((z - p.z - 1) + p.x);
    ry = F2 * (y - p.y - 1) + F3 * p.z + F3 * p.x;
  }

  inline void limits(pos_t &rx, pos_t &ry) const
  {
    rx = F1 * (z + x);
    ry = F2 * y + F3 * z + F3 * x;
  }
private:
  const pos_t x;
  const pos_t y;
  const pos_t z;

};

#endif /* _CUBE_H_ */
