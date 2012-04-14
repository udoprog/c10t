#ifndef __MARKER_HPP__
#define __MARKER_HPP__

#include <string>
#include <ostream>

#include <boost/foreach.hpp>

#include "settings_t.hpp"

#include "text.hpp"
#include "players.hpp"

#include "mc/blocks.hpp"
#include "mc/marker.hpp"
#include "mc/level_info.hpp"
#include "mc/rotated_level_info.hpp"
#include "mc/world.hpp"

struct marker {
public:
  marker(std::string text, std::string type, text::font_face font, int x, int y, int z);

  std::string get_text();
  std::string get_type();
  text::font_face get_font();

  int get_x();
  int get_y();
  int get_z();
private:
  std::string text;
  std::string type;
  text::font_face font;
  int x;
  int y;
  int z;
};

#endif /*__MARKER_HPP__*/
