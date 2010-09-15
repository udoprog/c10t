add_executable(c10t
	main.cpp
	Level.cpp
	Image.cpp
	Color.cpp
	blocks.cpp
  fileutils.cpp
	nbt/nbt.cpp)

# see /usr/share/cmake-2.8/Modules/Find*.cmake

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})
target_link_libraries(c10t ${ZLIB_LIBRARIES})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(c10t ${PNG_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(c10t ${CMAKE_THREAD_LIBS_INIT})

# set(Boost_DEBUG TRUE)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(c10t boost_thread)

