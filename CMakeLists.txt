cmake_minimum_required(VERSION 2.6)
project(c10t)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}")

# attempt to find out which revision we are building from
execute_process(COMMAND git rev-list HEAD -n1 --abbrev-commit
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE C10T_REVISION)

OPTION(BUILD_DEV_LIBS "Attempt to build libraries under ./libs" ON)

set(C10T_VERSION "git ${C10T_REVISION}")
set(C10T_SITE "http://github.com/udoprog/c10t")
set(C10T_CONTACT "Udoprog <johnjohn.tedro@gmail.com> et. al (see README)")
set(C10T_COMMENT "Created using c10t (http://github.com/udoprog/c10t)")

configure_file(${CMAKE_SOURCE_DIR}/src/config.hpp.cmake
  ${CMAKE_BINARY_DIR}/src/config.hpp)

include_directories(${CMAKE_BINARY_DIR}/src)
# output the binary to the build directory instead of in src/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

OPTION(Boost_USE_STATIC_LIBS "Use static boost libs" TRUE)

find_package(ZLIB REQUIRED)
find_package(PNG 1.2 REQUIRED)
find_package(Freetype 2 REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost 1.46 COMPONENTS thread filesystem system regex REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIR})
include_directories(${FREETYPE_INCLUDE_DIR_freetype2})
include_directories(${FREETYPE_INCLUDE_DIR_ft2build})
include_directories(${Boost_INCLUDE_DIR})

set(c10t_LIBRARIES
  dl
  # internal libraries that could/should be externalized
  c10t-image
  c10t-nbt
  c10t-mc
  c10t-engine
  # external libraries
  ${ZLIB_LIBRARIES}
  ${PNG_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
  ${FREETYPE_LIBRARY}
  )

if (BUILD_DEV_LIBS)
  include_directories("${CMAKE_SOURCE_DIR}/libs/unc/src")
  add_subdirectory(libs/unc)
  set(c10t_LIBRARIES ${c10t_LIBRARIES}
    unc
    uncdata
    )
else (BUILD_DEV_LIBS)
  find_package(PkgConfig)
  pkg_check_modules(UNC unc)
  set(c10t_LIBRARIES ${c10t_LIBRARIES} ${UNC_LIBRARIES})
endif (BUILD_DEV_LIBS)

add_subdirectory(src/image)
add_subdirectory(src/engine)
add_subdirectory(src/nbt)
add_subdirectory(src/mc)

add_subdirectory(test)

set(c10t_SOURCES 
  src/main.cpp
  src/dlopen.cpp
  src/main_utils.cpp
  src/algorithm.cpp
  src/players.cpp
  src/fileutils.cpp
  src/dirlist.cpp
  src/altitude_graph.cpp
  src/warps.cpp
  src/text.cpp
  src/json.cpp
  src/generate_map.cpp
  src/generate_statistics.cpp
  src/marker.cpp
  src/nullstream.cpp
  src/settings_t.cpp
  )

add_executable(c10t ${c10t_SOURCES})
add_executable(c10t-debug EXCLUDE_FROM_ALL ${c10t_SOURCES})
add_executable(nbt-inspect EXCLUDE_FROM_ALL src/nbt/nbt_inspect.cpp src/nbt/nbt.cpp)
add_executable(region-inspect EXCLUDE_FROM_ALL src/mc/region_inspect.cpp)

set_target_properties(c10t PROPERTIES COMPILE_FLAGS "-O3 -Wall -pedantic")
set_target_properties(c10t-debug PROPERTIES COMPILE_FLAGS "-Wall -pedantic -g")
set_target_properties(nbt-inspect PROPERTIES COMPILE_FLAGS "-O3 -Wall -pedantic")
set_target_properties(region-inspect PROPERTIES COMPILE_FLAGS "-O3 -Wall -pedantic")

target_link_libraries(c10t ${c10t_LIBRARIES})
target_link_libraries(c10t-debug ${c10t_LIBRARIES})
target_link_libraries(nbt-inspect ${ZLIB_LIBRARIES})
target_link_libraries(region-inspect ${c10t_LIBRARIES})
