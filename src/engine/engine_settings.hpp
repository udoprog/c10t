#ifndef _ENGINE_ENGINE_SETTINGS_HPP
#define _ENGINE_ENGINE_SETTINGS_HPP

#include <boost/shared_array.hpp>
#include "../selectors.hpp"

struct engine_settings {
  int rotation;
  bool night;
  bool heightmap;
  bool striped_terrain;
  bool hellmode;
  bool cavemode;

  pchunksel selector;

  int top;
  int bottom;
  boost::shared_array<bool> excludes;
};

#endif /* _ENGINE_ENGINE_SETTINGS_HPP */
