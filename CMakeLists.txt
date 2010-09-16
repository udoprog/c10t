cmake_minimum_required(VERSION 2.6)
project(c10t)

if (EXISTS .git/refs/heads/master)
  file(READ ".git/refs/heads/master" C10T_VERSION)
  string(REGEX REPLACE "\n" "" C10T_VERSION "${C10T_VERSION}")
else (EXISTS .git/refs/heads/master)
  set(C10T_VERSION "SNAPSHOT")
endif (EXISTS .git/refs/heads/master)

set(C10T_SITE "http://github.com/udoprog/c10t")
set(C10T_CONTACT "Udoprog <johnjohn.tedro@gmail.com>")

# output the binary to the current directory instead of in src/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

add_definitions("-Wall -pedantic -O3 -g")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)

add_subdirectory(src)
