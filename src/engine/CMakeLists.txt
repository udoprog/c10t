add_library(
  c10t-engine
  topdown_engine.cpp
  oblique_engine.cpp
  obliqueangle_engine.cpp
  isometric_engine.cpp
  fatiso_engine.cpp
  functions.cpp
  block_rotation.cpp
)

set_target_properties(c10t-engine PROPERTIES COMPILE_FLAGS "-O3 -Wall -pedantic -g")
