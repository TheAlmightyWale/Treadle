include(GoogleTest)

set(TARGET_NAME "TreadleTest")

add_executable(${TARGET_NAME}   "JobSystemTest.cpp"  "TaskTest.cpp" "WaitTest.cpp" "TestUtils.hpp")
set_target_properties(${TARGET_NAME}  PROPERTIES CXX_STANDARD 20)
target_include_directories(${TARGET_NAME}  PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(${TARGET_NAME}  PRIVATE gtest_main PUBLIC Treadle)

gtest_discover_tests(${TARGET_NAME})