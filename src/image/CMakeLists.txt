add_library(c10t-image 
  image_operations.cpp
  image_base.cpp
  memory_image.cpp
  cached_image.cpp
  color.cpp
  algorithms.cpp
  ${ZLIB_LIBRARIES} ${PNG_LIBRARIES})

set_target_properties(c10t-image PROPERTIES COMPILE_FLAGS "-O3 -Wall -pedantic -g")
