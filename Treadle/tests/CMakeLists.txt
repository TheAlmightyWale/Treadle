include(GoogleTest)

set(TARGET_NAME "TreadleTest")

add_executable(${TARGET_NAME}  "TaskTest.cpp" "JobSystemTest.cpp"  "Treadle2Test.cpp")
set_target_properties(${TARGET_NAME}  PROPERTIES CXX_STANDARD 20)
target_link_libraries(${TARGET_NAME}  PUBLIC gtest_main PUBLIC Treadle Treadle2)

gtest_discover_tests(${TARGET_NAME})