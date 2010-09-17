add_executable(c10t-test test.cpp)

target_link_libraries(c10t-test c10t-lib)
target_link_libraries(c10t-test ${c10t_LIBRARIES})
target_link_libraries(c10t-test boost_unit_test_framework)
