// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _CUBE_H_
#define _CUBE_H_

struct point {
  int x;
  int y;
  int z;

  inline point(int x, int y, int z) : x(x), y(y), z(z) {}
};

struct point2 {
  int x;
  int y;
};

class Cube {
public:
  const int x;
  const int y;
  const int z;
  
  Cube(const int x, const int y, const int z) : x(x), y(y), z(z) {}
  
  void project_top(point &p, int &rx, int &ry) {
    rx = z - p.z - 1;
    ry = p.x;
  }
  
  void project_oblique(point &p, int &rx, int &ry) {
    rx = z - p.z - 1;
    ry = p.x + (y - p.y - 1);
  }
  
  void project_obliqueangle(point &p, int &rx, int &ry) {
    rx = (z - p.z - 1) + p.x;
    ry = (y - p.y - 1) + p.z + p.x;
  }

  void project_isometric(point &p, int &rx, int &ry) {
    rx = 2 * ((z - p.z - 1) + p.x);
    ry = 2 * (y - p.y - 1) + p.z + p.x;
  }
  
  void get_top_limits(int &rx, int &ry) {
    rx = z;
    ry = x;
  }
  
  void get_oblique_limits(int &rx, int &ry) {
    rx = z;
    ry = x + y;
  }
  
  void get_obliqueangle_limits(int &rx, int &ry) {
    rx = z + x;
    ry = y + z + x;
  }
  
  void get_isometric_limits(int &rx, int &ry) {
    rx = 2 * (z + x);
    ry = 2 * y + z + x;
  }
};

#endif /* _CUBE_H_ */
