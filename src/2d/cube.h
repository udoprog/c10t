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
  
  /* perform a trivial top-down projection */
  void project_top(point &p, point2 &r) {
    r.x = p.x;
    r.y = p.z;
  }
  
  void project_top(point &p, int &rx, int &ry) {
    rx = p.x;
    ry = p.z;
  }
  
  void project_oblique(point &p, int &rx, int &ry) {
    rx = p.x;
    ry = p.z + (y - p.y);
  }
  
  void project_obliqueangle(point &p, point2 &r) {
    project_obliqueangle(p, r.x, r.y);
  }
  
  void project_obliqueangle(point &p, int &rx, int &ry) {
    rx = z + p.x - p.z;
    ry = (y - p.y) + p.z + p.x;
  }

  void project_isometric(point &p, int &rx, int &ry) {
    rx = 2 * (z + p.x - p.z);
    ry = 2 * (y - p.y) + p.z + p.x;
  }
  
  void get_top_limits(int &rx, int &ry) {
    rx = x;
    ry = z;
  }
  
  void get_oblique_limits(int &rx, int &ry) {
    rx = x;
    ry = z + y;
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
