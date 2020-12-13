#ifndef _ENGINE_ENGINE_SETTINGS_HPP
#define _ENGINE_ENGINE_SETTINGS_HPP

#include <boost/shared_array.hpp>

struct engine_settings {
  int rotation;
  bool night;
  bool heightmap;
  bool striped_terrain;
  bool hellmode;
  bool cavemode;
  int top;
  int bottom;
};

#endif /* _ENGINE_ENGINE_SETTINGS_HPP */
