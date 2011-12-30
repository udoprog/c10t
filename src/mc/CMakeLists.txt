add_library(
  c10t-mc
  blocks.cpp
  level.cpp
  utils.cpp
  world.cpp
  region.cpp
  level_info.cpp
  region_iterator.cpp
  rotated_level_info.cpp
  dynamic_buffer.cpp
  marker.cpp
)

set_target_properties(c10t-mc PROPERTIES COMPILE_FLAGS "-O3 -Wall -pedantic -g")
