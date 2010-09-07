#ifndef _COLOR_H_
#define _COLOR_H_

#include <stdint.h>

struct Color{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;

  Color() : r(255), g(255), b(255), a(0) { }
  
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
    r(r), g(g), b(b), a(a) {
  }
  
  ~Color(){
  }
};

#endif /* _COLOR_H_ */
