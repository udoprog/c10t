add_library(
  c10t-mc
  blocks.cpp
  level.cpp
  utils.cpp
  world.cpp
)

set_target_properties(c10t-mc PROPERTIES COMPILE_FLAGS "-O3 -Wall -pedantic -g")
