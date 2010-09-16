add_executable(c10t
	main.cpp
	Level.cpp
	Image.cpp
	Color.cpp
	blocks.cpp
  fileutils.cpp
	nbt/nbt.cpp)

add_executable(c10t-test
	test.cpp
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

target_link_libraries(c10t-test ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(c10t-test ${PNG_LIBRARIES})
target_link_libraries(c10t-test ${ZLIB_LIBRARIES})
target_link_libraries(c10t-test boost_thread)
target_link_libraries(c10t-test boost_unit_test_framework)
