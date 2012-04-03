// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/color.hpp"

#include <sstream>
#include <string>

float alpha_over_c(float u, float o, float ua, float oa);

/**
 * Takes two color values and does an alpha over blending without using floats.
 */
inline float alpha_over_c(float ac, float aa, float bc, float ba) {
  return (ac * aa) + (bc * ba - aa * bc * ba);
}

void color::blend(const color &other) {
  if (other.is_invisible()) return;

  if (other.is_opaque() || is_invisible()) {
    r = other.r;
    g = other.g;
    b = other.b;
    a = other.a;
    return;
  }
  
  r = alpha_over_c(other.r, other.a, r, a);
  g = alpha_over_c(other.g, other.a, g, a);
  b = alpha_over_c(other.b, other.a, b, a);
  a = a + other.a * (1.0f - a);
  r = ((r * 1.0f) / a);
  g = ((g * 1.0f) / a);
  b = ((b * 1.0f) / a);
}

// pull color down towards black
void color::darken(float f) {
  r = std::max(r - (r * f), 0.0f);
  g = std::max(g - (g * f), 0.0f);
  b = std::max(b - (b * f), 0.0f);
}

void color::darken(int f) {
  darken(float(f) / 255.0f);
}

void color::lighten(float f) {
  r = std::min(r + (r * f), 1.0f);
  g = std::min(g + (g * f), 1.0f);
  b = std::min(b + (b * f), 1.0f);
}

void color::lighten(int f) {
  lighten(float(f) / 255.0f);
}
