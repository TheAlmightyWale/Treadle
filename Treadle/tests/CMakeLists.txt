include(GoogleTest)

set(TARGET_NAME "TreadleTest")

add_executable(${TARGET_NAME}   "JobSystemTest.cpp"  "Treadle2Test.cpp" "WaitTest.cpp" "TestUtils.hpp" "MultiTaskTest.cpp")
set_target_properties(${TARGET_NAME}  PROPERTIES CXX_STANDARD 20)
target_include_directories(${TARGET_NAME}  PUBLIC "${CMAKE_SOURCE_DIR}/treadle2")
target_link_libraries(${TARGET_NAME}  PUBLIC gtest_main PUBLIC Treadle Treadle2)

gtest_discover_tests(${TARGET_NAME})