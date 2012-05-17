#include "marker.hpp"

marker::marker(std::string text, std::string type, text::font_face font, int x, int y, int z)
    : text(text),
      type(type),
      font(font),
      x(x),
      y(y),
      z(z)
{
}

std::string marker::get_text() {
  return text;
}

std::string marker::get_type() {
  return type;
}

text::font_face marker::get_font() {
  return font;
}

int marker::get_x() {
  return x;
}

int marker::get_y() {
  return y;
}

int marker::get_z() {
  return z;
}
