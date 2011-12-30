cmake_minimum_required(VERSION 2.6)
project(c10t)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules;${CMAKE_MODULE_PATH}")

# attempt to find out which revision we are building from
execute_process(COMMAND git rev-list HEAD -n1 --abbrev-commit
				OUTPUT_STRIP_TRAILING_WHITESPACE
				OUTPUT_VARIABLE C10T_REVISION)
set(C10T_VERSION "git ${C10T_REVISION}")
set(C10T_SITE "http://github.com/udoprog/c10t")
set(C10T_CONTACT "Udoprog <johnjohn.tedro@gmail.com> et. al (see README)")
set(C10T_COMMENT "Created using c10t (http://github.com/udoprog/c10t)")

configure_file(${CMAKE_SOURCE_DIR}/src/config.hpp.cmake
  ${CMAKE_BINARY_DIR}/src/config.hpp)
include_directories(${CMAKE_BINARY_DIR}/src)

# output the binary to the build directory instead of in src/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(FreeType REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost COMPONENTS thread filesystem system REQUIRED)

include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIR})
include_directories(${FREETYPE_INCLUDE_DIR_freetype2})
include_directories(${FREETYPE_INCLUDE_DIR_ft2build})
include_directories(${Boost_INCLUDE_DIR})

set(c10t_LIBRARIES ${ZLIB_LIBRARIES} ${PNG_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
set(c10t_LIBRARIES ${c10t_LIBRARIES} ${Boost_LIBRARIES} ${FREETYPE_LIBRARY})

# set(Boost_DEBUG TRUE)
include_directories("${CMAKE_SOURCE_DIR}/src/")

#copy font.ttg to build directory as matter of convenience.
#execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/font.ttf ${CMAKE_BINARY_DIR}/font.ttf)

add_subdirectory(libs/unc)
include_directories("${CMAKE_SOURCE_DIR}/libs/unc/src")

add_subdirectory(src/image)
add_subdirectory(src/nbt)
add_subdirectory(src/mc)

set(c10t_SOURCES src/algorithm.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/players.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/fileutils.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/dirlist.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/altitude_graph.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/warps.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/text.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/json.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/engine/topdown_engine.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/engine/oblique_engine.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/engine/obliqueangle_engine.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/engine/isometric_engine.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/engine/fatiso_engine.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/engine/functions.cpp)
set(c10t_SOURCES ${c10t_SOURCES} src/engine/block_rotation.cpp)

set(c10t_LIBRARIES ${c10t_LIBRARIES} unc uncdata c10t-image c10t-nbt c10t-mc)

add_executable(c10t src/main.cpp src/main_utils.cpp ${c10t_SOURCES})
add_executable(c10t-debug EXCLUDE_FROM_ALL src/main.cpp src/main_utils.cpp ${c10t_SOURCES})
add_executable(nbt-inspect EXCLUDE_FROM_ALL src/nbt/nbt_inspect.cpp src/nbt/nbt.cpp)
add_executable(region-inspect EXCLUDE_FROM_ALL src/mc/region_inspect.cpp)

set_target_properties(c10t PROPERTIES COMPILE_FLAGS "-O3 -Wall")
set_target_properties(c10t-debug PROPERTIES COMPILE_FLAGS "-Wall -pedantic -g")
set_target_properties(nbt-inspect PROPERTIES COMPILE_FLAGS "-O3 -Wall -pedantic -g")

target_link_libraries(c10t ${c10t_LIBRARIES})
target_link_libraries(c10t-debug ${c10t_LIBRARIES})
target_link_libraries(nbt-inspect ${ZLIB_LIBRARIES})
target_link_libraries(region-inspect ${c10t_LIBRARIES})

add_subdirectory(test)
