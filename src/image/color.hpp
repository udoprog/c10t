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
  float r;
  float g;
  float b;
  float a;
  int32_t z;

  color(color *c)
    : r(c->r),
      g(c->g),
      b(c->b),
      a(c->a),
      z(0)
  {
  }

  color()
    : r(1.0f),
      g(1.0f),
      b(1.0f),
      a(0.0f),
      z(0)
  {
  }

  color(int r, int g, int b, int a)
    : r(float(r) / 255.0f),
      g(float(g) / 255.0f),
      b(float(b) / 255.0f),
      a(float(a) / 255.0f),
      z(0)
  {
  }

  color(float r, float g, float b, float a)
    : r(r),
      g(g),
      b(b),
      a(a),
      z(0)
  {
  }

  bool is_opaque() const {
    return a == 1.0f;
  }

  bool is_transparent() const {
    return a != 1.0f;
  }

  bool is_invisible() const {
    return a == 0.0f;
  }

  ~color(){
  }

  void darken(float c);
  void darken(int c);
  void lighten(float c);
  void lighten(int c);
  void blend(const color &other);

  inline void read(color *buf) {
    r = buf->r;
    g = buf->g;
    b = buf->b;
    a = buf->a;
  }

  inline void write(color *buf) {
    buf->r = r;
    buf->g = g;
    buf->b = b;
    buf->a = a;
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

  bool operator==(const color &c) const
  {
    return r == c.r && g == c.g && b == c.b && a == c.a;
  }
};

#endif /* COLOR_HPP */
