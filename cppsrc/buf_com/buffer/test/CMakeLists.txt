enable_testing()

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(buffer_test buffer_test.cpp)

target_link_libraries(buffer_test Boost::unit_test_framework pthread)

add_test(NAME buffer_test COMMAND buffer_test)