add_library(c10t-nbt
  nbt.cpp
  ${ZLIB_LIBRARIES})

set_target_properties(c10t-nbt PROPERTIES COMPILE_FLAGS "-O3 -Wall -pedantic -g")
