// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef COLOR_HPP
#define COLOR_HPP

#include <stdint.h>
#include <assert.h>

#include <string>
#include <sstream>
#include <ostream>

#include <boost/lexical_cast.hpp>

struct color{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
  
  color(color *c) : r(c->r), g(c->g), b(c->b), a(c->a) { }
  
  color() : r(0xff), g(0xff), b(0xff), a(0x00) { }
  
  color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
    r(r), g(g), b(b), a(a) {
  }
  
  bool is_opaque() const {
    return a == 0xff;
  }
  
  bool is_transparent() const {
    return a != 0xff;
  }
  
  bool is_invisible() const {
    return a == 0x00;
  }
  
  ~color(){
  }
  
  void darken(uint8_t c);
  void lighten(uint8_t c);
  void blend(const color &other);

  inline void read(uint8_t *buf) {
    r = buf[0];
    g = buf[1];
    b = buf[2];
    a = buf[3];
  }
  
  inline void write(uint8_t *buf) {
    buf[0] = r;
    buf[1] = g;
    buf[2] = b;
    buf[3] = a;
  }
  
  friend std::ostream& operator<<(std::ostream& out, const color& c) // output
  {
    std::stringstream ss;
    ss << (int)c.r << "," << (int)c.g << "," << (int)c.b << "," << (int)c.a;
    out << ss.str();
    return out;
  }

  bool operator!=(const color &c) const
  {
    return !(r == c.r && g == c.g && b == c.b && a == c.a);
  }
};

#endif /* COLOR_HPP */
