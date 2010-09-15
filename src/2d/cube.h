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
    r.x = x + p.x - p.z;
    r.y = y + p.x - p.y + p.z;
  }
  
  void project_obliqueangle(point &p, int &rx, int &ry) {
    rx = x + p.x - p.z;
    ry = y + p.x - p.y + p.z;
  }
};

#endif /* _CUBE_H_ */
