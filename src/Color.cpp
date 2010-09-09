#include "Color.h"

/**
 * Takes two color values and does an alpha over blending without using floats.
 */
uint8_t alpha_over_c(uint8_t u, uint8_t o, uint8_t oa) {
  uint8_t d = (u < o) ? o - u : u - o;
  d = ((0xff - oa) * d) / 0xff;
  
  if (u < o) {
    u += d;
  }
  else {
    u -= d;
  }
  
  return u;
}

Color Color::overlay(const Color &other) {
  Color c;
  c.r = alpha_over_c(r, other.r, other.a);
  c.g = alpha_over_c(g, other.g, other.a);
  c.b = alpha_over_c(b, other.b, other.a);
  // blend the alpha channels
  c.a = a - (a * (0xff - other.a)) / 0xff;
  return c;
}
