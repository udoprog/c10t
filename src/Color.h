#ifndef _COLOR_H_
#define _COLOR_H_

#include <stdint.h>

#include <string>
#include <ostream>

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
  
  Color(uint32_t rgba) :
    r(r >> 24), g((g >> 16) & 0xff), b((b >> 8) & 0xff), a(a & 0xff) {
  }

  bool is_opaque() const {
    return a == 0xff;
  }
  
  bool is_transparent() const {
    return a == 0x00;
  }
  
  ~Color(){
  }
  
  void overlay(const Color &other);
  void underlay(const Color &other);
  void darken(uint8_t c);
  void blend(const Color &other);
    
  friend std::ostream& operator<<(std::ostream& out, const Color& c) // output
  {
      out << "Color(" << (int)c.r << ", " << (int)c.g << ", " << (int)c.b << ", " << (int)c.a << ")";
      return out;
  }
};

#endif /* _COLOR_H_ */
