#ifndef _COLOR_H_
#define _COLOR_H_

#include <stdint.h>
#include <string>

uint8_t alpha_over_c(uint8_t u, uint8_t o, uint8_t ua, uint8_t oa);

struct Color{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
  
  Color(Color *c) : r(c->r), g(c->g), b(c->b), a(c->a) { }
  
  Color() : r(0xff), g(0xff), b(0xff), a(0x00) { }
  
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
    r(r), g(g), b(b), a(a) {
  }
  
  ~Color(){
  }
  
  void overlay(const Color &other);
  void underlay(const Color &other);

  std::string to_s();
};

#endif /* _COLOR_H_ */
