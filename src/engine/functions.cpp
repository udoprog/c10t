#include "engine/functions.hpp"

void apply_shading(
        const engine_settings& s,
        int block_light,
        int sky_light,
        int height_map,
        int y,
        color &c)
{
  if(s.night) {
    c.darken(0x6 * (16 - block_light));
  }
  else if (sky_light != -1 && y != s.top) {
    c.darken(0x6 * (16 - std::max(sky_light, block_light)));
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
}
