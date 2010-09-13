cmake_minimum_required(VERSION 2.6)
project(c10t)

# output the binary to the current directory instead of in src/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

add_definitions("-Wall -pedantic -O3")

add_subdirectory(src)
