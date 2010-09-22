set(c10t_SOURCES Level.cpp)
set(c10t_SOURCES ${c10t_SOURCES} Image.cpp)
set(c10t_SOURCES ${c10t_SOURCES} Color.cpp)
set(c10t_SOURCES ${c10t_SOURCES} blocks.cpp)
set(c10t_SOURCES ${c10t_SOURCES} fileutils.cpp)
set(c10t_SOURCES ${c10t_SOURCES} nbt/nbt.cpp)

add_library(c10t-lib ${c10t_SOURCES})
add_executable(c10t main.cpp ${c10t_SOURCES})
add_executable(c10t-debug EXCLUDE_FROM_ALL main.cpp ${c10t_SOURCES})

set_target_properties(c10t PROPERTIES COMPILE_FLAGS "-O3 -Wall -pedantic")
set_target_properties(c10t-debug PROPERTIES COMPILE_FLAGS "-Wall -pedantic -g")
set_target_properties(c10t-lib PROPERTIES COMPILE_FLAGS "-O3 -Wall -pedantic -g")

target_link_libraries(c10t ${c10t_LIBRARIES})
target_link_libraries(c10t-debug ${c10t_LIBRARIES})
