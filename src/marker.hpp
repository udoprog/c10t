#ifndef MARKER
#define MARKER

#include <string>

#include "text.hpp"

struct marker {
public:
  std::string text;
  std::string type;
  text::font_face font;
  int x, y, z;
  
  marker(std::string text, std::string type, text::font_face font, int x, int y, int z) :
      text(text), type(type), font(font), x(x), y(y), z(z)
  {
  }
};

struct light_marker {
public:
  std::string text;
  int x, y, z;
  
  light_marker(std::string text, int x, int y, int z) :
      text(text), x(x), y(y), z(z)
  {
  }
};

#endif
