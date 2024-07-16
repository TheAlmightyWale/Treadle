include(GoogleTest)

set(TARGET_NAME "TreadleTest")

add_executable(${TARGET_NAME}  "TaskTest.cpp" "JobSystemTest.cpp" )
set_property(TARGET ${TARGET_NAME}  PROPERTY CXX_STANDARD 20)
target_link_libraries(${TARGET_NAME}  PUBLIC gtest_main PUBLIC Treadle)

gtest_discover_tests(${TARGET_NAME})