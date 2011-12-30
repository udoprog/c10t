#include "engine/functions.hpp"

void apply_shading(const engine_settings& s, int bl, int sl, int hm, int y, color &c)
{
  // if night, darken all colors not emitting light
  if (bl == -1) {
    bl = 0;
  }
  
  if(s.night) {
    c.darken(0xa * (16 - bl));
  }
  else if (sl != -1 && y != s.top) {
    c.darken(0xa * (16 - std::max(sl, bl)));
  }
  
  //c.darken((mc::MapY - y));
  
  // in heightmap mode, brightness = height
  if (s.heightmap) {
    c.b = y*2;
    c.g = y*2;
    c.r = y*2;
    c.a = 0xff;
  }
  else if (s.striped_terrain && y % 2 == 0) {
    c.darken(0xf);
  }
  else {
    c.darken((mc::MapY - y));
  }
}
