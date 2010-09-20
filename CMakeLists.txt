cmake_minimum_required(VERSION 2.6)
project(c10t)

# attempt to find out which revision we are building from
set(C10T_VERSION "1.0")
set(C10T_SITE "http://github.com/udoprog/c10t")
set(C10T_CONTACT "Udoprog <johnjohn.tedro@gmail.com> et. al (see README)")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)

# output the binary to the current directory instead of in src/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost COMPONENTS thread REQUIRED)

include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})

set(c10t_LIBRARIES ${ZLIB_LIBRARIES} ${PNG_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
set(c10t_LIBRARIES ${c10t_LIBRARIES} ${Boost_LIBRARIES})

# set(Boost_DEBUG TRUE)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/")

add_subdirectory(src)
add_subdirectory(test)
