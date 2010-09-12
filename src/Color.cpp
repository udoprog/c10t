#include "Color.h"
#include <sstream>
#include <string>

/**
 * Takes two color values and does an alpha over blending without using floats.
 */
uint8_t alpha_over_c(uint8_t ac, uint8_t aa, uint8_t bc, uint8_t ba) {
  uint8_t caa = (ac * aa) / 0xff;
  uint8_t cba = (bc * ba) / 0xff;
  return (caa + ((0xff - aa) * cba) / 0xff);
}

void Color::overlay(const Color &other) {
  r = alpha_over_c(other.r, other.a, r, a);
  g = alpha_over_c(other.g, other.a, g, a);
  b = alpha_over_c(other.b, other.a, b, a);
  a = a + (other.a * (0xff - a)) / 0xff;
}

void Color::underlay(const Color &other) {
  r = alpha_over_c(r, a, other.r, other.a);
  g = alpha_over_c(g, a, other.g, other.a);
  b = alpha_over_c(b, a, other.b, other.a);
  a = other.a + (a * (0xff - other.a)) / 0xff;
}

std::string Color::to_s() {
  std::stringstream ss(std::stringstream::out | std::stringstream::in);
  ss << "(" << (int)r << "," << (int)g << "," << (int)b << "," << (int)a << ")";
  std::string s = ss.str();
  return s;
}
